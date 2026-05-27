#include "Point.h"

#include "GeometryTypes.h"

#include <cmath>
#include <iomanip>
#include <ostream>

namespace geometry {

// ----------------------------------------------------------------------------
// Writes a point in "(x, y)" format for console output.
// ----------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& os, const Point& p)
{
  os << '(' << std::setprecision(15) << p.x << ", " << p.y << ')';
  return os;
}

// ----------------------------------------------------------------------------
// Cross product of AB x AC. The sign tells whether a->b->c is a left turn
// (positive), right turn (negative), or nearly collinear (zero).
// ----------------------------------------------------------------------------
double Cross(const Point& a, const Point& b, const Point& c)
{
  return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
}

// ----------------------------------------------------------------------------
// Compares two points with a tolerance instead of exact floating-point equality.
// ----------------------------------------------------------------------------
bool NearlyEqual(const Point& a, const Point& b)
{
  return std::abs(a.x - b.x) <= kEps && std::abs(a.y - b.y) <= kEps;
}

}  // namespace geometry
