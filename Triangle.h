#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "Point.h"

#include <vector>

namespace geometry {

struct Triangle {
  Point a;
  Point b;
  Point c;
};

double TriangleArea(const Triangle& t);
double SumTriangleAreas(const std::vector<Triangle>& triangles);

}  // namespace geometry

#endif  // TRIANGLE_H
