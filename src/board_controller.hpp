#pragma once

#include <mutex>

#include "board_view.hpp"
#include "timer.hpp"

class BoardController {
 public:
  BoardController(BoardView& board_view,
                  BoardModel& board_model,
                  ScoreModel& score_model);

  void Start();

 private:
  BoardModel& board_model_;
  ScoreModel& score_model_;
  BoardView& board_view_;
  timer::Timer timer_;
  std::mutex mutex_;
  int interval_time_{500};
  int one_line_score_{100};
  int move_down_score_{1};
  int lock_delay_{200};
  bool is_bypass_control_{false};
};
