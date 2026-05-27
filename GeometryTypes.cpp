#include "GeometryTypes.h"
#include "Polygon.h"

namespace geometry {

// ----------------------------------------------------------------------------
// Returns the previous index in a circular polygon vertex list.
// ----------------------------------------------------------------------------
size_t PreviousIndex(size_t i, size_t n)
{
  return (i + n - 1) % n;
}

// ----------------------------------------------------------------------------
// Returns the next index in a circular polygon vertex list.
// ----------------------------------------------------------------------------
size_t NextIndex(size_t i, size_t n)
{
  return (i + 1) % n;
}

// ----------------------------------------------------------------------------
// Polygon positive area means counter-clockwise vertex order;
// Polygon negative area means clockwise vertex order.
// ----------------------------------------------------------------------------
double PolygonSignedArea(const Polygon& poly)
{
  return poly.SignedArea();
}

}  // namespace geometry
