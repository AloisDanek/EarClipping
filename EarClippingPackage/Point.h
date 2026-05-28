#ifndef POINT_H
#define POINT_H

#include <iosfwd>

namespace geometry {

class Point {
 public:
  Point() = default;
  Point(double x, double y);

  double Cross(const Point& b, const Point& c) const;
  bool NearlyEquals(const Point& other) const;

  double x = 0.0;
  double y = 0.0;
};

std::ostream& operator<<(std::ostream& os, const Point& p);

}  // namespace geometry

#endif  // POINT_H
