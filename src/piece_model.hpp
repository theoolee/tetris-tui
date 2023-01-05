#pragma once

#include <memory>
#include "common.hpp"

enum class PieceType {
  kI,
  kSquare,
  kL,
  kLMirrored,
  kN,
  kT,
};

class PieceModel {
 public:
  explicit PieceModel(PieceType type);
  PieceModel(const PieceModel& other);
  static std::unique_ptr<PieceModel> Random();

  [[nodiscard]] const Face& GetData() const;
  [[nodiscard]] const Point& GetAnchorOffset() const;
  [[nodiscard]] const Dimension& GetDimension() const;
  void Rotate();

 private:
  Dimension dimension_{0, 0};
  Point anchor_offset_{0, 0};
  std::unique_ptr<Face> data_ptr_;

  void UpdateMetadata();
};
