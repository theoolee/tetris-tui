#pragma once

#include <mutex>

#include "board_view.hpp"
#include "timer.hpp"

class board_controller {
 public:
  board_controller(board_view& board_view,
                   board_model& board_model,
                   score_model& score_model);

  void start();

 private:
  board_model& board_model_;
  score_model& score_model_;
  board_view& board_view_;
  timer::timer timer_;
  std::mutex mutex_;
  int interval_time_{500};
  int one_line_score_{100};
  int move_down_score_{1};
  int lock_delay_{200};
  bool is_bypass_control_{false};
};
