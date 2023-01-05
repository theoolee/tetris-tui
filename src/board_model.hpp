#pragma once

#include <memory>
#include "common.hpp"
#include "piece_model.hpp"

class BoardModel {
 public:
  explicit BoardModel(const Dimension& dimension);

  [[nodiscard]] const Face& GetBaseData() const;
  [[nodiscard]] const Face& GetPieceData() const;
  [[nodiscard]] const Face& GetGhostPieceData() const;
  [[nodiscard]] const PieceModel& GetNextPiece() const;
  bool MovePiece(const Point& position);
  bool RotatePiece();
  void NewPiece();
  void LockPiece();
  int ClearLines();

 private:
  Point piece_anchor_position_{0, 0};
  Dimension dimension_{0, 0};
  std::unique_ptr<Face> base_data_ptr_;
  std::unique_ptr<Face> piece_data_ptr_;
  std::unique_ptr<Face> ghost_piece_data_ptr_;
  std::unique_ptr<PieceModel> piece_ptr_{PieceModel::Random()};
  std::unique_ptr<PieceModel> next_piece_ptr_{PieceModel::Random()};

  void ResetPieceAnchorPosition();
  void UpdateData();
  [[nodiscard]] bool TestCollision(const PieceModel& piece,
                                   const Point& piece_anchor_position) const;
};
