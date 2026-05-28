#include "Bounds.h"

#include "GeometryTypes.h"

namespace geometry {

// ----------------------------------------------------------------------------
// Creates an axis-aligned bounding box from explicit min and max coordinates.
// ----------------------------------------------------------------------------
Bounds::Bounds(double min_x, double max_x, double min_y, double max_y)
    : min_x_(min_x), max_x_(max_x), min_y_(min_y), max_y_(max_y)
{
}

// ----------------------------------------------------------------------------
// Returns the minimum x coordinate.
// ----------------------------------------------------------------------------
double Bounds::MinX() const
{
  return min_x_;
}

// ----------------------------------------------------------------------------
// Returns the maximum x coordinate.
// ----------------------------------------------------------------------------
double Bounds::MaxX() const
{
  return max_x_;
}

// ----------------------------------------------------------------------------
// Returns the minimum y coordinate.
// ----------------------------------------------------------------------------
double Bounds::MinY() const
{
  return min_y_;
}

// ----------------------------------------------------------------------------
// Returns the maximum y coordinate.
// ----------------------------------------------------------------------------
double Bounds::MaxY() const
{
  return max_y_;
}

// ----------------------------------------------------------------------------
// Returns true when a point lies inside or on the bounding box.
// ----------------------------------------------------------------------------
bool Bounds::Contains(const Point& point) const
{
  return point.x >= min_x_ - kEps && point.x <= max_x_ + kEps &&
         point.y >= min_y_ - kEps && point.y <= max_y_ + kEps;
}

}  // namespace geometry
