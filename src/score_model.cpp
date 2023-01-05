#include "score_model.hpp"

const int& score_model::get_data() const {
  return data_;
}

void score_model::add(int score) {
  data_ += score;
}

void score_model::clear() {
  data_ = 0;
}
