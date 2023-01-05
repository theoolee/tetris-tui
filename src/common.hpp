#pragma once

#include <vector>

using line = std::vector<unsigned char>;
using face = std::vector<line>;

struct dimension {
  int width;
  int height;

  dimension operator+(const dimension& dimension) const {
    return {width + dimension.width, height + dimension.height};
  }
};

struct point {
  int x;
  int y;

  point operator+(const point& position) const {
    return {x + position.x, y + position.y};
  }

  point operator+(const dimension& dimension) const {
    return {x + dimension.width, y + dimension.height};
  }

  point operator-(const point& position) const {
    return {x - position.x, y - position.y};
  }

  point& operator+=(const point& position) {
    x += position.x;
    y += position.y;

    return *this;
  }
};
