#ifndef POINT_H_
#define POINT_H_

struct Point {
  float x;
  float y;

  explicit Point(float x = 0, float y = 0) : x(x), y(y) {}
};

#endif  // POINT_H_
