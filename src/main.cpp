#include "board_controller.hpp"
#include "board_model.hpp"
#include "board_view.hpp"
#include "score_model.hpp"

int main() {
  const Dimension board_dimension = {15, 25};
  ScoreModel score_model;
  BoardModel board_model{board_dimension};
  BoardView board_view{board_dimension, board_model, score_model};
  BoardController board_controller{board_view, board_model, score_model};

  board_controller.Start();
}
