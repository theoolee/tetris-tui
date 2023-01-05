#include "board_view.hpp"

board_view::board_view(const dimension& dimension,
                       board_model& board_model,
                       score_model& score_model)
    : screen_{ftxui::ScreenInteractive::FixedSize(dimension.width * 2 + 2,
                                                  dimension.height + 10)},
      dimension_{dimension},
      board_model_{board_model},
      score_model_{score_model} {}

void board_view::set_event_handler(
    const std::shared_ptr<event_handler>& handler) {
  event_handler_ = handler;
}

void board_view::render() {
  std::cout << "\x1b[2j\x1b[h";

  ftxui::Component component = ftxui::Renderer([&]() {
    return ftxui::window(
        ftxui::text("Tetris"),
        ftxui::vbox({
            ftxui::hbox({
                render_score() |
                    ftxui::size(ftxui::WIDTH, ftxui::EQUAL, dimension_.width),
                ftxui::separator(),
                render_next_piece() |
                    ftxui::size(ftxui::WIDTH, ftxui::EQUAL, dimension_.width),
            }) | ftxui::flex,
            ftxui::separator(),
            render_board() |
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

void board_view::redraw() {
  screen_.PostEvent(ftxui::Event::Custom);
}

ftxui::Element board_view::render_score() {
  const int& score = score_model_.get_data();

  return ftxui::vbox({
      ftxui::text("score:") | ftxui::center | ftxui::bgcolor(ftxui::Color::Red),
      ftxui::text(std::to_string(score)) | ftxui::center | ftxui::flex,
  });
}

ftxui::Element board_view::render_next_piece() {
  const piece_model& piece = board_model_.get_next_piece();
  const face& piece_data = piece.get_data();
  const dimension& piece_dimension = piece.get_dimension();
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
      ftxui::text("next:") | ftxui::center | ftxui::bgcolor(ftxui::Color::Blue),
      ftxui::gridbox(render_data) | ftxui::center | ftxui::flex,
  });
}

ftxui::Element board_view::render_board() {
  const face& base_data = board_model_.get_base_data();
  const face& piece_data = board_model_.get_piece_data();
  const face& ghost_piece_data = board_model_.get_ghost_piece_data();
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
