#include "board_controller.hpp"

BoardController::BoardController(BoardView& board_view,
                                 BoardModel& board_model,
                                 ScoreModel& score_model)
    : board_view_(board_view),
      board_model_(board_model),
      score_model_(score_model) {}

void BoardController::Start() {
  auto lock_piece = [&]() {
    board_model_.LockPiece();
    is_bypass_control_ = true;
    timer_.SetTimeout(
        std::make_shared<std::function<void()>>([&]() {
          mutex_.lock();
          score_model_.Add(board_model_.ClearLines() * one_line_score_);
          board_model_.NewPiece();
          board_view_.Redraw();
          is_bypass_control_ = false;
          mutex_.unlock();
        }),
        lock_delay_);
  };
  auto interval_callback = std::make_shared<std::function<void()>>([&]() {
    mutex_.lock();

    if (!is_bypass_control_) {
      if (!board_model_.MovePiece({0, 1})) {
        lock_piece();
      }

      board_view_.Redraw();
    }

    mutex_.unlock();
  });
  auto id = timer_.SetInterval(interval_callback, interval_time_);
  auto event_handler =
      std::make_shared<EventHandler>([&](const ftxui::Event& event) {
        mutex_.lock();

        if (!is_bypass_control_) {
          if (event == ftxui::Event::ArrowLeft) {
            board_model_.MovePiece({-1, 0});
          } else if (event == ftxui::Event::ArrowRight) {
            board_model_.MovePiece({1, 0});
          } else if (event == ftxui::Event::ArrowUp) {
            board_model_.RotatePiece();
          } else if (event == ftxui::Event::ArrowDown) {
            if (board_model_.MovePiece({0, 1})) {
              score_model_.Add(move_down_score_);
              timer_.Clear(id);
              id = timer_.SetInterval(interval_callback, interval_time_);
            } else {
              lock_piece();
            }
          }
        }

        mutex_.unlock();

        return false;
      });

  board_view_.SetEventHandler(event_handler);
  board_view_.Render();
}
