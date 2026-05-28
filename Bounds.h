#ifndef BOUNDS_H
#define BOUNDS_H

#include "Point.h"

namespace geometry {

class Bounds {
 public:
  Bounds() = default;
  Bounds(double min_x, double max_x, double min_y, double max_y);

  bool Contains(const Point& point) const;

 private:
  double min_x_ = 0.0;
  double max_x_ = 0.0;
  double min_y_ = 0.0;
  double max_y_ = 0.0;
};

}  // namespace geometry

#endif  // BOUNDS_H
