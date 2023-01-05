#include "score_model.hpp"

const int& ScoreModel::GetData() const {
  return data_;
}

void ScoreModel::Add(int score) {
  data_ += score;
}

void ScoreModel::Clear() {
  data_ = 0;
}
