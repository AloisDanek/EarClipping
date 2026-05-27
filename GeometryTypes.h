#ifndef GEOMETRY_TYPES_H
#define GEOMETRY_TYPES_H

#include <cstddef>
#include <iosfwd>
#include <vector>

namespace geometry {

constexpr double kEps = 1.0e-12;

struct Point {
  double x = 0.0;
  double y = 0.0;
};

struct Triangle {
  Point a;
  Point b;
  Point c;
};

class Polygon;

std::ostream& operator<<(std::ostream& os, const Point& p);

size_t PreviousIndex(size_t i, size_t n);
size_t NextIndex(size_t i, size_t n);
double Cross(const Point& a, const Point& b, const Point& c);
double PolygonSignedArea(const Polygon& poly);
double TriangleArea(const Triangle& t);
double SumTriangleAreas(const std::vector<Triangle>& triangles);
bool NearlyEqual(const Point& a, const Point& b);

}  // namespace geometry

#endif  // GEOMETRY_TYPES_H
