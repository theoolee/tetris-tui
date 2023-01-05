#include "board_model.hpp"

constexpr point k_wall_kick_to_try[] = {
    {0, 0}, {1, 0}, {-1, 0}, {0, 1}, {0, -1}, {2, 0}, {-2, 0}, {0, 2}, {0, -2},
};

board_model::board_model(const dimension& dimension)
    : dimension_{dimension},
      base_data_ptr_{new face(dimension.height, line(dimension.width))},
      piece_data_ptr_{new face(dimension.height, line(dimension.width))},
      ghost_piece_data_ptr_{new face(dimension.height, line(dimension.width))} {
  reset_piece_anchor_position();
  update_data();
}

const face& board_model::get_base_data() const {
  return *base_data_ptr_;
}

const face& board_model::get_piece_data() const {
  return *piece_data_ptr_;
}

const face& board_model::get_ghost_piece_data() const {
  return *ghost_piece_data_ptr_;
}

const piece_model& board_model::get_next_piece() const {
  return *next_piece_ptr_;
}

bool board_model::move_piece(const point& position) {
  const point new_piece_anchor_position = piece_anchor_position_ + position;

  if (test_collision(*piece_ptr_, new_piece_anchor_position)) {
    return false;
  }

  piece_anchor_position_ = new_piece_anchor_position;
  update_data();

  return true;
}

bool board_model::rotate_piece() {
  std::unique_ptr<piece_model> new_piece_ptr{new piece_model{*piece_ptr_}};
  new_piece_ptr->rotate();

  // nolintnextline
  for (const auto& position : k_wall_kick_to_try) {
    if (!test_collision(*new_piece_ptr, piece_anchor_position_ + position)) {
      piece_ptr_ = std::move(new_piece_ptr);
      piece_anchor_position_ += position;
      update_data();

      return true;
    }
  }

  return false;
}

void board_model::new_piece() {
  piece_ptr_ = piece_model::random();
  std::swap(piece_ptr_, next_piece_ptr_);
  reset_piece_anchor_position();
  update_data();
}

void board_model::lock_piece() {
  for (int y = 0; y < dimension_.height; ++y) {
    for (int x = 0; x < dimension_.width; ++x) {
      if ((*piece_data_ptr_)[y][x] != 0) {
        (*base_data_ptr_)[y][x] = 1;
        (*piece_data_ptr_)[y][x] = 0;
        (*ghost_piece_data_ptr_)[y][x] = 0;
      }
    }
  }
}

int board_model::clear_lines() {
  int y = dimension_.height - 1;
  int cleared_number = 0;

  while (y >= 0) {
    bool is_line_full = true;

    for (int x = 0; x < dimension_.width; x++) {
      if ((*base_data_ptr_)[y][x] == 0) {
        is_line_full = false;
        break;
      }
    }

    if (is_line_full) {
      cleared_number++;
    } else if (cleared_number > 0) {
      (*base_data_ptr_)[y + cleared_number] = (*base_data_ptr_)[y];
    }

    y--;
  }

  return cleared_number;
}

void board_model::reset_piece_anchor_position() {
  piece_anchor_position_.x = dimension_.width / 2;
  piece_anchor_position_.y = piece_ptr_->get_anchor_offset().y;
}

void board_model::update_data() {
  const face& piece_data = piece_ptr_->get_data();
  const point& piece_anchor_offset = piece_ptr_->get_anchor_offset();
  const dimension& piece_dimension = piece_ptr_->get_dimension();
  const point piece_start_position =
      piece_anchor_position_ - piece_anchor_offset;
  const point piece_end_position = piece_start_position + piece_dimension;

  for (int y = 0; y < dimension_.height; y++) {
    for (int x = 0; x < dimension_.width; x++) {
      if (x >= piece_start_position.x && x < piece_end_position.x &&
          y >= piece_start_position.y && y < piece_end_position.y) {
        const int piece_x = x - piece_start_position.x;
        const int piece_y = y - piece_start_position.y;

        (*piece_data_ptr_)[y][x] = piece_data[piece_y][piece_x];
      } else {
        (*piece_data_ptr_)[y][x] = 0;
      }
    }
  }

  *ghost_piece_data_ptr_ = *piece_data_ptr_;
  int piece_preview_offset = dimension_.height - piece_end_position.y;

  for (int y = 0; y < piece_dimension.height; y++) {
    for (int x = 0; x < piece_dimension.width; x++) {
      if (piece_data[y][x] > 0) {
        const point base_start_position =
            piece_start_position + point{x, y + 1};

        for (int base_y = base_start_position.y; base_y < dimension_.height;
             base_y++) {
          if ((*base_data_ptr_)[base_y][base_start_position.x] > 0) {
            piece_preview_offset =
                std::min(base_y - base_start_position.y, piece_preview_offset);
          }
        }
      }
    }
  }

  if (piece_preview_offset > 0) {
    for (int y = piece_start_position.y; y < piece_end_position.y; y++) {
      for (int x = piece_start_position.x; x < piece_end_position.x; x++) {
        (*ghost_piece_data_ptr_)[y][x] = 0;
        (*ghost_piece_data_ptr_)[y + piece_preview_offset][x] =
            (*piece_data_ptr_)[y][x];
      }
    }
  }
}

bool board_model::test_collision(const piece_model& piece,
                                 const point& piece_anchor_position) const {
  const face& piece_data = piece.get_data();
  const dimension& piece_dimension = piece.get_dimension();
  const point& piece_anchor_offset = piece.get_anchor_offset();
  const point piece_start_position =
      piece_anchor_position - piece_anchor_offset;
  const point piece_end_position = piece_start_position + piece_dimension;

  if (piece_start_position.x < 0 || piece_end_position.x > dimension_.width ||
      piece_start_position.y < 0 || piece_end_position.y > dimension_.height) {
    return true;
  }

  for (int y = 0; y < piece_dimension.height; y++) {
    for (int x = 0; x < piece_dimension.width; x++) {
      if (piece_data[y][x] > 0) {
        const int base_x = piece_start_position.x + x;
        const int base_y = piece_start_position.y + y;

        if (base_x >= 0 && base_x < dimension_.width && base_y >= 0 &&
            base_y < dimension_.height &&
            (*base_data_ptr_)[base_y][base_x] > 0) {
          return true;
        }
      }
    }
  }

  return false;
}
