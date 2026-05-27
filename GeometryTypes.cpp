#include "GeometryTypes.h"
#include "Polygon.h"

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
// Cross product of AB x AC. The sign tells whether a->b->c is a left turn
// (positive), right turn (negative), or nearly collinear (zero).
// ----------------------------------------------------------------------------
double Cross(const Point& a, const Point& b, const Point& c)
{
  return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
}

// ----------------------------------------------------------------------------
// Polygon positive area means counter-clockwise vertex order;
// Polygon negative area means clockwise vertex order.
// ----------------------------------------------------------------------------
double PolygonSignedArea(const Polygon& poly)
{
  return poly.SignedArea();
}

// ----------------------------------------------------------------------------
// Returns the absolute area of one triangle.
// ----------------------------------------------------------------------------
double TriangleArea(const Triangle& t)
{
  return std::abs(Cross(t.a, t.b, t.c)) * 0.5;
}

// ----------------------------------------------------------------------------
// Adds the areas of all triangles in a triangulation.
// ----------------------------------------------------------------------------
double SumTriangleAreas(const std::vector<Triangle>& triangles)
{
  double area = 0.0;
  for (const Triangle& t : triangles) area += TriangleArea(t);
  return area;
}

// ----------------------------------------------------------------------------
// Compares two points with a tolerance instead of exact floating-point equality.
// ----------------------------------------------------------------------------
bool NearlyEqual(const Point& a, const Point& b)
{
  return std::abs(a.x - b.x) <= kEps && std::abs(a.y - b.y) <= kEps;
}

}  // namespace geometry
