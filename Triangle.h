#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "Point.h"

#include <vector>

namespace geometry {

class Triangle {
 public:
  Triangle() = default;
  Triangle(Point a, Point b, Point c);

  double Area() const;

  Point a;
  Point b;
  Point c;
};

double SumTriangleAreas(const std::vector<Triangle>& triangles);

}  // namespace geometry

#endif  // TRIANGLE_H
