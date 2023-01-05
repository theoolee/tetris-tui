#include "board_view.hpp"

BoardView::BoardView(const Dimension& dimension,
                     BoardModel& board_model,
                     ScoreModel& score_model)
    : screen_{ftxui::ScreenInteractive::FixedSize(dimension.width * 2 + 2,
                                                  dimension.height + 10)},
      dimension_{dimension},
      board_model_{board_model},
      score_model_{score_model} {}

void BoardView::SetEventHandler(const std::shared_ptr<EventHandler>& handler) {
  event_handler_ = handler;
}

void BoardView::Render() {
  std::cout << "\x1B[2J\x1B[H";

  ftxui::Component component = ftxui::Renderer([&]() {
    return ftxui::window(
        ftxui::text("Tetris"),
        ftxui::vbox({
            ftxui::hbox({
                RenderScore() |
                    ftxui::size(ftxui::WIDTH, ftxui::EQUAL, dimension_.width),
                ftxui::separator(),
                RenderNextPiece() |
                    ftxui::size(ftxui::WIDTH, ftxui::EQUAL, dimension_.width),
            }) | ftxui::flex,
            ftxui::separator(),
            RenderBoard() |
                ftxui::size(ftxui::WIDTH, ftxui::EQUAL, dimension_.width * 2) |
                ftxui::size(ftxui::HEIGHT, ftxui::EQUAL, dimension_.height),
        }));
  });

  if (event_handler_ != nullptr) {
    screen_.Loop(component | ftxui::CatchEvent(*event_handler_));
  } else {
    screen_.Loop(component);
  }
}

void BoardView::Redraw() {
  screen_.PostEvent(ftxui::Event::Custom);
}

ftxui::Element BoardView::RenderScore() {
  const int& score = score_model_.GetData();

  return ftxui::vbox({
      ftxui::text("Score:") | ftxui::center | ftxui::bgcolor(ftxui::Color::Red),
      ftxui::text(std::to_string(score)) | ftxui::center | ftxui::flex,
  });
}

ftxui::Element BoardView::RenderNextPiece() {
  const PieceModel& piece = board_model_.GetNextPiece();
  const Face& piece_data = piece.GetData();
  const Dimension& piece_dimension = piece.GetDimension();
  std::vector<ftxui::Elements> render_data;

  for (int y = 0; y < piece_dimension.height; y++) {
    ftxui::Elements render_line;

    for (int x = 0; x < piece_dimension.width; x++) {
      if (piece_data[y][x] > 0) {
        render_line.push_back(ftxui::text("██"));
      } else {
        render_line.push_back(ftxui::text("  "));
      }
    }

    render_data.push_back(render_line);
  }

  return ftxui::vbox({
      ftxui::text("Next:") | ftxui::center | ftxui::bgcolor(ftxui::Color::Blue),
      ftxui::gridbox(render_data) | ftxui::center | ftxui::flex,
  });
}

ftxui::Element BoardView::RenderBoard() {
  const Face& base_data = board_model_.GetBaseData();
  const Face& piece_data = board_model_.GetPieceData();
  const Face& ghost_piece_data = board_model_.GetGhostPieceData();
  std::vector<ftxui::Elements> render_data;

  for (int y = 0; y < dimension_.height; ++y) {
    ftxui::Elements render_line;

    for (int x = 0; x < dimension_.width; ++x) {
      if (piece_data[y][x] > 0) {
        render_line.push_back(ftxui::text("██") |
                              ftxui::color(ftxui::Color::Blue));
      } else if (ghost_piece_data[y][x] > 0) {
        render_line.push_back(ftxui::text("▓▓") |
                              ftxui::color(ftxui::Color::Grey35));
      } else if (base_data[y][x] > 0) {
        render_line.push_back(ftxui::text("██") |
                              ftxui::color(ftxui::Color::Red));
      } else {
        render_line.push_back(ftxui::text("  "));
      }
    }

    render_data.push_back(render_line);
  }

  return ftxui::gridbox(render_data);
}
