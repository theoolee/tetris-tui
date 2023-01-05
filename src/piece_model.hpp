#pragma once

#include <memory>
#include "common.hpp"

enum class piece_type {
  k_i,
  k_square,
  k_l,
  k_l_mirrored,
  k_n,
  k_t,
};

class piece_model {
 public:
  explicit piece_model(piece_type type);
  piece_model(const piece_model& other);
  static std::unique_ptr<piece_model> random();

  [[nodiscard]] const face& get_data() const;
  [[nodiscard]] const point& get_anchor_offset() const;
  [[nodiscard]] const dimension& get_dimension() const;
  void rotate();

 private:
  dimension dimension_{0, 0};
  point anchor_offset_{0, 0};
  std::unique_ptr<face> data_ptr_;

  void update_metadata();
};
