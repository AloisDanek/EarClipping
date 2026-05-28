#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "Point.h"

#include <vector>

namespace geometry {

class Bounds;

class Triangle {
 public:
  Triangle() = default;
  Triangle(Point a, Point b, Point c);

  double Area() const;
  Bounds GetBounds() const;
  bool IsInside(const Point& point) const;

  Point a;
  Point b;
  Point c;
};

double SumTriangleAreas(const std::vector<Triangle>& triangles);

}  // namespace geometry

#endif  // TRIANGLE_H
