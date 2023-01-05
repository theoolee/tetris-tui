#include "board_controller.hpp"

board_controller::board_controller(board_view& board_view,
                                   board_model& board_model,
                                   score_model& score_model)
    : board_view_(board_view),
      board_model_(board_model),
      score_model_(score_model) {}

void board_controller::start() {
  auto lock_piece = [&]() {
    board_model_.lock_piece();
    is_bypass_control_ = true;
    timer_.set_timeout(
        std::make_shared<std::function<void()>>([&]() {
          mutex_.lock();
          score_model_.add(board_model_.clear_lines() * one_line_score_);
          board_model_.new_piece();
          board_view_.redraw();
          is_bypass_control_ = false;
          mutex_.unlock();
        }),
        lock_delay_);
  };
  auto interval_callback = std::make_shared<std::function<void()>>([&]() {
    mutex_.lock();

    if (!is_bypass_control_) {
      if (!board_model_.move_piece({0, 1})) {
        lock_piece();
      }

      board_view_.redraw();
    }

    mutex_.unlock();
  });
  auto id = timer_.set_interval(interval_callback, interval_time_);
  auto event_handler =
      std::make_shared<::event_handler>([&](const ftxui::Event& event) {
        mutex_.lock();

        if (!is_bypass_control_) {
          if (event == ftxui::Event::ArrowLeft) {
            board_model_.move_piece({-1, 0});
          } else if (event == ftxui::Event::ArrowRight) {
            board_model_.move_piece({1, 0});
          } else if (event == ftxui::Event::ArrowUp) {
            board_model_.rotate_piece();
          } else if (event == ftxui::Event::ArrowDown) {
            if (board_model_.move_piece({0, 1})) {
              score_model_.add(move_down_score_);
              timer_.clear(id);
              id = timer_.set_interval(interval_callback, interval_time_);
            } else {
              lock_piece();
            }
          }
        }

        mutex_.unlock();

        return false;
      });

  board_view_.set_event_handler(event_handler);
  board_view_.render();
}
