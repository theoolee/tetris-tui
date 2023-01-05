#pragma once

class score_model {
 public:
  [[nodiscard]] const int& get_data() const;
  void add(int score);
  void clear();

 private:
  int data_ = 0;
};
