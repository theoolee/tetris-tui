#pragma once

class ScoreModel {
 public:
  [[nodiscard]] const int& GetData() const;
  void Add(int score);
  void Clear();

 private:
  int data_ = 0;
};
