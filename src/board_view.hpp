#pragma once

#include <ftxui/component/component.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/component/screen_interactive.hpp>

#include <memory>
#include "board_model.hpp"
#include "score_model.hpp"

using EventHandler = std::function<bool(ftxui::Event)>;

class BoardView {
 public:
  BoardView(const Dimension& dimension,
            BoardModel& board_model,
            ScoreModel& score_model);

  void SetEventHandler(const std::shared_ptr<EventHandler>& handler);
  void Render();
  void Redraw();

 private:
  BoardModel& board_model_;
  ScoreModel& score_model_;
  std::shared_ptr<EventHandler> event_handler_;
  Dimension dimension_;
  ftxui::ScreenInteractive screen_;

  ftxui::Element RenderScore();
  ftxui::Element RenderNextPiece();
  ftxui::Element RenderBoard();
};
