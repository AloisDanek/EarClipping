#include "Polygon.h"

#include <algorithm>
#include <utility>

namespace geometry {
namespace {

struct Bounds {
  double min_x = 0.0;
  double max_x = 0.0;
  double min_y = 0.0;
  double max_y = 0.0;
};

// ----------------------------------------------------------------------------
// Computes the axis-aligned bounding box for a triangle.
// ----------------------------------------------------------------------------
Bounds TriangleBounds(const Triangle& t)
{
  return {std::min({t.a.x, t.b.x, t.c.x}),
          std::max({t.a.x, t.b.x, t.c.x}),
          std::min({t.a.y, t.b.y, t.c.y}),
          std::max({t.a.y, t.b.y, t.c.y})};
}

// ----------------------------------------------------------------------------
// Returns true when a point lies inside or on a bounding box.
// ----------------------------------------------------------------------------
bool PointInBounds(const Point& p, const Bounds& bounds)
{
  return p.x >= bounds.min_x - kEps && p.x <= bounds.max_x + kEps &&
         p.y >= bounds.min_y - kEps && p.y <= bounds.max_y + kEps;
}

// ----------------------------------------------------------------------------
// For a counter-clockwise triangle, a point is inside or on the boundary
// when it is always on the left side of each directed triangle edge.
// ----------------------------------------------------------------------------
bool PointInTriangle(const Point& p, const Triangle& t)
{
  const double c1 = t.a.Cross(t.b, p);
  const double c2 = t.b.Cross(t.c, p);
  const double c3 = t.c.Cross(t.a, p);
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
  if (ear.a.Cross(ear.b, ear.c) <= kEps) {
    return false;
  }

  const Bounds bounds = TriangleBounds(ear);
  for (size_t j = 0; j < vertices_.size(); ++j) {
    if (j == i || j == PreviousIndex(i, vertices_.size()) ||
        j == NextIndex(i, vertices_.size())) {
      continue;
    }
    if (!PointInBounds(vertices_[j], bounds)) {
      continue;
    }
    if (PointInTriangle(vertices_[j], ear)) {
      return false;
    }
  }
  return true;
}

}  // namespace geometry
