#include "Point.h"

#include "GeometryTypes.h"

#include <cmath>
#include <iomanip>
#include <ostream>

namespace geometry {

// ----------------------------------------------------------------------------
// Creates a point at the supplied x and y coordinates.
// ----------------------------------------------------------------------------
Point::Point(double x, double y) : x(x), y(y) {}

// ----------------------------------------------------------------------------
// Cross product of AB x AC. The sign tells whether this->b->c is a left turn
// (positive), right turn (negative), or nearly collinear (zero).
// ----------------------------------------------------------------------------
double Point::Cross(const Point& b, const Point& c) const
{
  return (b.x - x) * (c.y - y) - (b.y - y) * (c.x - x);
}

// ----------------------------------------------------------------------------
// Compares this point with another using a floating-point tolerance.
// ----------------------------------------------------------------------------
bool Point::NearlyEquals(const Point& other) const
{
  return std::abs(x - other.x) <= kEps && std::abs(y - other.y) <= kEps;
}

// ----------------------------------------------------------------------------
// Writes a point in "(x, y)" format for console output.
// ----------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& os, const Point& p)
{
  os << '(' << std::setprecision(15) << p.x << ", " << p.y << ')';
  return os;
}

}  // namespace geometry
