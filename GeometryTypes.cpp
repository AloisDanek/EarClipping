#include "GeometryTypes.h"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <ostream>
#include <utility>

namespace geometry {
namespace {

// ----------------------------------------------------------------------------
// For a counter-clockwise triangle, a point is inside or on the boundary
// when it is always on the left side of each directed triangle edge.
// ----------------------------------------------------------------------------
bool PointInTriangle(const Point& p, const Triangle& t)
{
  const double c1 = Cross(t.a, t.b, p);
  const double c2 = Cross(t.b, t.c, p);
  const double c3 = Cross(t.c, t.a, p);
  return c1 >= -kEps && c2 >= -kEps && c3 >= -kEps;
}

}  // namespace

// ----------------------------------------------------------------------------
// Creates a polygon from an initializer list of vertices.
// ----------------------------------------------------------------------------
Polygon::Polygon(std::initializer_list<Point> vertices) : vertices_(vertices) {}

// ----------------------------------------------------------------------------
// Creates a polygon by taking ownership of an existing vertex vector.
// ----------------------------------------------------------------------------
Polygon::Polygon(std::vector<Point> vertices) : vertices_(std::move(vertices)) {}

// ----------------------------------------------------------------------------
// Returns the number of vertices in the polygon.
// ----------------------------------------------------------------------------
size_t Polygon::size() const
{
  return vertices_.size();
}

// ----------------------------------------------------------------------------
// Returns true when the polygon has no vertices.
// ----------------------------------------------------------------------------
bool Polygon::empty() const
{
  return vertices_.empty();
}

// ----------------------------------------------------------------------------
// Returns the vertex at index i.
// ----------------------------------------------------------------------------
const Point& Polygon::operator[](size_t i) const
{
  return vertices_[i];
}

// ----------------------------------------------------------------------------
// Returns the mutable vertex at index i.
// ----------------------------------------------------------------------------
Point& Polygon::operator[](size_t i)
{
  return vertices_[i];
}

// ----------------------------------------------------------------------------
// Returns the first vertex in the polygon.
// ----------------------------------------------------------------------------
const Point& Polygon::front() const
{
  return vertices_.front();
}

// ----------------------------------------------------------------------------
// Returns the last vertex in the polygon.
// ----------------------------------------------------------------------------
const Point& Polygon::back() const
{
  return vertices_.back();
}

// ----------------------------------------------------------------------------
// Returns an iterator to the first vertex.
// ----------------------------------------------------------------------------
std::vector<Point>::const_iterator Polygon::begin() const
{
  return vertices_.begin();
}

// ----------------------------------------------------------------------------
// Returns an iterator one past the last vertex.
// ----------------------------------------------------------------------------
std::vector<Point>::const_iterator Polygon::end() const
{
  return vertices_.end();
}

// ----------------------------------------------------------------------------
// Appends a vertex to the polygon.
// ----------------------------------------------------------------------------
void Polygon::push_back(const Point& point)
{
  vertices_.push_back(point);
}

// ----------------------------------------------------------------------------
// Removes the last vertex from the polygon.
// ----------------------------------------------------------------------------
void Polygon::pop_back()
{
  vertices_.pop_back();
}

// ----------------------------------------------------------------------------
// Removes the vertex at index i.
// ----------------------------------------------------------------------------
void Polygon::erase(size_t i)
{
  vertices_.erase(vertices_.begin() + static_cast<std::ptrdiff_t>(i));
}

// ----------------------------------------------------------------------------
// Reverses the polygon vertex order.
// ----------------------------------------------------------------------------
void Polygon::reverse()
{
  std::reverse(vertices_.begin(), vertices_.end());
}

// ----------------------------------------------------------------------------
// Returns the signed polygon area. Positive means counter-clockwise order.
// ----------------------------------------------------------------------------
double Polygon::SignedArea() const
{
  double area2 = 0.0;
  for (size_t i = 0; i < vertices_.size(); ++i) {
    const Point& p = vertices_[i];
    const Point& q = vertices_[NextIndex(i, vertices_.size())];
    area2 += p.x * q.y - p.y * q.x;
  }
  return 0.5 * area2;
}

// ----------------------------------------------------------------------------
// Returns the vertex before index i, wrapping from vertex 0 to the last vertex.
// ----------------------------------------------------------------------------
const Point& Polygon::PreviousVertex(size_t i) const
{
  return vertices_[PreviousIndex(i, vertices_.size())];
}

// ----------------------------------------------------------------------------
// Returns the vertex after index i, wrapping from the last vertex to vertex 0.
// ----------------------------------------------------------------------------
const Point& Polygon::NextVertex(size_t i) const
{
  return vertices_[NextIndex(i, vertices_.size())];
}

// ----------------------------------------------------------------------------
// Builds the possible ear triangle at vertex i.
// ----------------------------------------------------------------------------
Triangle Polygon::EarCandidateAt(size_t i) const
{
  return {PreviousVertex(i), vertices_[i], NextVertex(i)};
}

// ----------------------------------------------------------------------------
// Returns true when vertex i is a valid ear of the current polygon.
// ----------------------------------------------------------------------------
bool Polygon::IsEar(size_t i) const
{
  const Triangle ear = EarCandidateAt(i);
  if (Cross(ear.a, ear.b, ear.c) <= kEps) {
    return false;
  }

  for (size_t j = 0; j < vertices_.size(); ++j) {
    if (j == i || j == PreviousIndex(i, vertices_.size()) ||
        j == NextIndex(i, vertices_.size())) {
      continue;
    }
    if (PointInTriangle(vertices_[j], ear)) {
      return false;
    }
  }
  return true;
}

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
