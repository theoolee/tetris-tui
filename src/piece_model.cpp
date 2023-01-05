#include "piece_model.hpp"

const std::vector<Face> kShape = {
    // I
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
    // L
    {
        {1, 0},
        {2, 0},
        {1, 1},
    },
    // L mirrored
    {
        {0, 1},
        {0, 2},
        {1, 1},
    },
    // N
    {
        {0, 1},
        {2, 1},
        {1, 0},
    },
    // N mirrored
    {
        {1, 0},
        {1, 2},
        {0, 1},
    },
    // T
    {
        {1, 2, 1},
        {0, 1, 0},
    },
};

PieceModel::PieceModel(PieceType shapeType)
    : data_ptr_{new Face{kShape[static_cast<int>(shapeType)]}} {
  UpdateMetadata();
}

PieceModel::PieceModel(const PieceModel& other)
    : data_ptr_{new Face{*other.data_ptr_}} {
  UpdateMetadata();
}

std::unique_ptr<PieceModel> PieceModel::Random() {
  srand(static_cast<unsigned int>(
      std::chrono::system_clock::now().time_since_epoch().count()));
  std::unique_ptr<PieceModel> piece_ptr{
      new PieceModel{static_cast<PieceType>(rand() % kShape.size())}};

  for (int i = 0; i < rand() % 4; i++) {
    piece_ptr->Rotate();
  }

  return piece_ptr;
}

const Face& PieceModel::GetData() const {
  return *data_ptr_;
}

const Point& PieceModel::GetAnchorOffset() const {
  return anchor_offset_;
}

const Dimension& PieceModel::GetDimension() const {
  return dimension_;
}

void PieceModel::Rotate() {
  int width;
  int new_width;
  int height;
  int new_height;
  width = new_height = (*data_ptr_)[0].size();
  height = new_width = (*data_ptr_).size();
  std::unique_ptr<Face> new_shape_ptr{new Face{
      static_cast<size_t>(new_height),
      Line(new_width),
  }};

  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      (*new_shape_ptr)[x][height - y - 1] = (*data_ptr_)[y][x];
    }
  }

  data_ptr_ = std::move(new_shape_ptr);
  UpdateMetadata();
}

void PieceModel::UpdateMetadata() {
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
