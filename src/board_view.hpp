#pragma once

#include "ftxui/component/component.hpp"
#include "ftxui/component/event.hpp"
#include "ftxui/component/screen_interactive.hpp"

#include <memory>
#include "board_model.hpp"
#include "score_model.hpp"

using event_handler = std::function<bool(ftxui::Event)>;

class board_view {
 public:
  board_view(const dimension& dimension,
             board_model& board_model,
             score_model& score_model);

  void set_event_handler(const std::shared_ptr<event_handler>& handler);
  void render();
  void redraw();

 private:
  board_model& board_model_;
  score_model& score_model_;
  std::shared_ptr<event_handler> event_handler_;
  dimension dimension_;
  ftxui::ScreenInteractive screen_;

  ftxui::Element render_score();
  ftxui::Element render_next_piece();
  ftxui::Element render_board();
};
