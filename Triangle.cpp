#include "Triangle.h"

#include <cmath>

namespace geometry {

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

}  // namespace geometry
