#include "board_model.hpp"

constexpr Point kWallKickToTry[] = {
    {0, 0}, {1, 0}, {-1, 0}, {0, 1}, {0, -1}, {2, 0}, {-2, 0}, {0, 2}, {0, -2},
};

BoardModel::BoardModel(const Dimension& dimension)
    : dimension_{dimension},
      base_data_ptr_{new Face(dimension.height, Line(dimension.width))},
      piece_data_ptr_{new Face(dimension.height, Line(dimension.width))},
      ghost_piece_data_ptr_{new Face(dimension.height, Line(dimension.width))} {
  ResetPieceAnchorPosition();
  UpdateData();
}

const Face& BoardModel::GetBaseData() const {
  return *base_data_ptr_;
}

const Face& BoardModel::GetPieceData() const {
  return *piece_data_ptr_;
}

const Face& BoardModel::GetGhostPieceData() const {
  return *ghost_piece_data_ptr_;
}

const PieceModel& BoardModel::GetNextPiece() const {
  return *next_piece_ptr_;
}

bool BoardModel::MovePiece(const Point& position) {
  const Point new_piece_anchor_position = piece_anchor_position_ + position;

  if (TestCollision(*piece_ptr_, new_piece_anchor_position)) {
    return false;
  }

  piece_anchor_position_ = new_piece_anchor_position;
  UpdateData();

  return true;
}

bool BoardModel::RotatePiece() {
  std::unique_ptr<PieceModel> new_piece_ptr{new PieceModel{*piece_ptr_}};
  new_piece_ptr->Rotate();

  // NOLINTNEXTLINE
  for (const auto& position : kWallKickToTry) {
    if (!TestCollision(*new_piece_ptr, piece_anchor_position_ + position)) {
      piece_ptr_ = std::move(new_piece_ptr);
      piece_anchor_position_ += position;
      UpdateData();

      return true;
    }
  }

  return false;
}

void BoardModel::NewPiece() {
  piece_ptr_ = PieceModel::Random();
  std::swap(piece_ptr_, next_piece_ptr_);
  ResetPieceAnchorPosition();
  UpdateData();
}

void BoardModel::LockPiece() {
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

int BoardModel::ClearLines() {
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

void BoardModel::ResetPieceAnchorPosition() {
  piece_anchor_position_.x = dimension_.width / 2;
  piece_anchor_position_.y = piece_ptr_->GetAnchorOffset().y;
}

void BoardModel::UpdateData() {
  const Face& piece_data = piece_ptr_->GetData();
  const Point& piece_anchor_offset = piece_ptr_->GetAnchorOffset();
  const Dimension& piece_dimension = piece_ptr_->GetDimension();
  const Point piece_start_position =
      piece_anchor_position_ - piece_anchor_offset;
  const Point piece_end_position = piece_start_position + piece_dimension;

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
        const Point base_start_position =
            piece_start_position + Point{x, y + 1};

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

bool BoardModel::TestCollision(const PieceModel& piece,
                               const Point& piece_anchor_position) const {
  const Face& piece_data = piece.GetData();
  const Dimension& piece_dimension = piece.GetDimension();
  const Point& piece_anchor_offset = piece.GetAnchorOffset();
  const Point piece_start_position =
      piece_anchor_position - piece_anchor_offset;
  const Point piece_end_position = piece_start_position + piece_dimension;

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
