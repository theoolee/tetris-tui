#include "piece_model.hpp"

const std::vector<face> k_shape = {
    // i
    {
        {1},
        {2},
        {1},
        {1},
    },
    // square
    {
        {2, 2},
        {2, 2},
    },
    // l
    {
        {1, 0},
        {2, 0},
        {1, 1},
    },
    // l mirrored
    {
        {0, 1},
        {0, 2},
        {1, 1},
    },
    // n
    {
        {0, 1},
        {2, 1},
        {1, 0},
    },
    // n mirrored
    {
        {1, 0},
        {1, 2},
        {0, 1},
    },
    // t
    {
        {1, 2, 1},
        {0, 1, 0},
    },
};

piece_model::piece_model(piece_type shape_type)
    : data_ptr_{new face{k_shape[static_cast<int>(shape_type)]}} {
  update_metadata();
}

piece_model::piece_model(const piece_model& other)
    : data_ptr_{new face{*other.data_ptr_}} {
  update_metadata();
}

std::unique_ptr<piece_model> piece_model::random() {
  srand(static_cast<unsigned int>(
      std::chrono::system_clock::now().time_since_epoch().count()));
  std::unique_ptr<piece_model> piece_ptr{
      new piece_model{static_cast<piece_type>(rand() % k_shape.size())}};

  for (int i = 0; i < rand() % 4; i++) {
    piece_ptr->rotate();
  }

  return piece_ptr;
}

const face& piece_model::get_data() const {
  return *data_ptr_;
}

const point& piece_model::get_anchor_offset() const {
  return anchor_offset_;
}

const dimension& piece_model::get_dimension() const {
  return dimension_;
}

void piece_model::rotate() {
  int width;
  int new_width;
  int height;
  int new_height;
  width = new_height = (*data_ptr_)[0].size();
  height = new_width = (*data_ptr_).size();
  std::unique_ptr<face> new_shape_ptr{new face{
      static_cast<size_t>(new_height),
      line(new_width),
  }};

  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      (*new_shape_ptr)[x][height - y - 1] = (*data_ptr_)[y][x];
    }
  }

  data_ptr_ = std::move(new_shape_ptr);
  update_metadata();
}

void piece_model::update_metadata() {
  dimension_ = {static_cast<int>((*data_ptr_)[0].size()),
                static_cast<int>((*data_ptr_).size())};

  for (int y = 0; y < dimension_.height; y++) {
    for (int x = 0; x < dimension_.width; x++) {
      if ((*data_ptr_)[y][x] == 2) {
        anchor_offset_ = {x, y};
        return;
      }
    }
  }
}
