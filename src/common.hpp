#pragma once

#include <vector>

using Line = std::vector<unsigned char>;
using Face = std::vector<Line>;

struct Dimension {
  int width;
  int height;

  Dimension operator+(const Dimension& dimension) const {
    return {width + dimension.width, height + dimension.height};
  }
};

struct Point {
  int x;
  int y;

  Point operator+(const Point& position) const {
    return {x + position.x, y + position.y};
  }

  Point operator+(const Dimension& dimension) const {
    return {x + dimension.width, y + dimension.height};
  }

  Point operator-(const Point& position) const {
    return {x - position.x, y - position.y};
  }

  Point& operator+=(const Point& position) {
    x += position.x;
    y += position.y;

    return *this;
  }
};
