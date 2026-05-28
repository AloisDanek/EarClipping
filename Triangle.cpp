#include "Bounds.h"
#include "Triangle.h"

#include <algorithm>
#include <cmath>
#include <utility>

namespace geometry {

// ----------------------------------------------------------------------------
// Creates a triangle from three points.
// ----------------------------------------------------------------------------
Triangle::Triangle(Point a, Point b, Point c)
    : a(std::move(a)), b(std::move(b)), c(std::move(c))
{
}

// ----------------------------------------------------------------------------
// Returns the absolute area of this triangle.
// ----------------------------------------------------------------------------
double Triangle::Area() const
{
  return std::abs(a.Cross(b, c)) * 0.5;
}

// ----------------------------------------------------------------------------
// Computes the axis-aligned bounding box for this triangle.
// ----------------------------------------------------------------------------
Bounds Triangle::GetBounds() const
{
  return {std::min({a.x, b.x, c.x}),
          std::max({a.x, b.x, c.x}),
          std::min({a.y, b.y, c.y}),
          std::max({a.y, b.y, c.y})};
}

// ----------------------------------------------------------------------------
// Adds the areas of all triangles in a triangulation.
// ----------------------------------------------------------------------------
double SumTriangleAreas(const std::vector<Triangle>& triangles)
{
  double area = 0.0;
  for (const Triangle& t : triangles) area += t.Area();
  return area;
}

}  // namespace geometry
