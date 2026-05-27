#ifndef POINT_H
#define POINT_H

#include <iosfwd>

namespace geometry {

struct Point {
  double x = 0.0;
  double y = 0.0;
};

std::ostream& operator<<(std::ostream& os, const Point& p);

double Cross(const Point& a, const Point& b, const Point& c);
bool NearlyEqual(const Point& a, const Point& b);

}  // namespace geometry

#endif  // POINT_H
