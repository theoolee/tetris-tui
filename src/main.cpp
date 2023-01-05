#include "board_controller.hpp"
#include "board_model.hpp"
#include "board_view.hpp"
#include "score_model.hpp"

int main() {
  const dimension board_dimension = {15, 25};
  score_model score_model;
  board_model board_model{board_dimension};
  board_view board_view{board_dimension, board_model, score_model};
  board_controller board_controller{board_view, board_model, score_model};

  board_controller.start();
}
