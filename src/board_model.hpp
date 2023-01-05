#pragma once

#include <memory>
#include "common.hpp"
#include "piece_model.hpp"

class board_model {
 public:
  explicit board_model(const dimension& dimension);

  [[nodiscard]] const face& get_base_data() const;
  [[nodiscard]] const face& get_piece_data() const;
  [[nodiscard]] const face& get_ghost_piece_data() const;
  [[nodiscard]] const piece_model& get_next_piece() const;
  bool move_piece(const point& position);
  bool rotate_piece();
  void new_piece();
  void lock_piece();
  int clear_lines();

 private:
  point piece_anchor_position_{0, 0};
  dimension dimension_{0, 0};
  std::unique_ptr<face> base_data_ptr_;
  std::unique_ptr<face> piece_data_ptr_;
  std::unique_ptr<face> ghost_piece_data_ptr_;
  std::unique_ptr<piece_model> piece_ptr_{piece_model::random()};
  std::unique_ptr<piece_model> next_piece_ptr_{piece_model::random()};

  void reset_piece_anchor_position();
  void update_data();
  [[nodiscard]] bool test_collision(const piece_model& piece,
                                    const point& piece_anchor_position) const;
};
