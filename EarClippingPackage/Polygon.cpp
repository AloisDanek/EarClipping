#include "Bounds.h"
#include "KDTree.h"
#include "Polygon.h"

#include <algorithm>
#include <stdexcept>
#include <utility>

namespace geometry {

// ----------------------------------------------------------------------------
// Creates an empty polygon.
// ----------------------------------------------------------------------------
Polygon::Polygon() = default;

// ----------------------------------------------------------------------------
// Creates a polygon from an initializer list of vertices.
// ----------------------------------------------------------------------------
Polygon::Polygon(std::initializer_list<Point> vertices) : vertices_(vertices) {}

// ----------------------------------------------------------------------------
// Creates a polygon by taking ownership of an existing vertex vector.
// ----------------------------------------------------------------------------
Polygon::Polygon(std::vector<Point> vertices) : vertices_(std::move(vertices)) {}

// ----------------------------------------------------------------------------
// Copies polygon vertices. Clipping state is rebuilt only when requested.
// ----------------------------------------------------------------------------
Polygon::Polygon(const Polygon& other) : vertices_(other.vertices_) {}

// ----------------------------------------------------------------------------
// Copies polygon vertices and clears any existing clipping state.
// ----------------------------------------------------------------------------
Polygon& Polygon::operator=(const Polygon& other)
{
  if (this != &other) {
    vertices_ = other.vertices_;
    ResetClippingState();
  }
  return *this;
}

// ----------------------------------------------------------------------------
// Allows unique_ptr<KDTree> to be destroyed with the complete KDTree type.
// ----------------------------------------------------------------------------
Polygon::~Polygon() = default;

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
  ResetClippingState();
}

// ----------------------------------------------------------------------------
// Removes the last vertex from the polygon.
// ----------------------------------------------------------------------------
void Polygon::pop_back()
{
  vertices_.pop_back();
  ResetClippingState();
}

// ----------------------------------------------------------------------------
// Removes the vertex at index i.
// ----------------------------------------------------------------------------
void Polygon::erase(size_t i)
{
  vertices_.erase(vertices_.begin() + static_cast<std::ptrdiff_t>(i));
  ResetClippingState();
}

// ----------------------------------------------------------------------------
// Reverses the polygon vertex order.
// ----------------------------------------------------------------------------
void Polygon::reverse()
{
  std::reverse(vertices_.begin(), vertices_.end());
  ResetClippingState();
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
// Computes the axis-aligned bounding box for the polygon.
// ----------------------------------------------------------------------------
Bounds Polygon::GetBounds() const
{
  double min_x = vertices_.front().x;
  double max_x = vertices_.front().x;
  double min_y = vertices_.front().y;
  double max_y = vertices_.front().y;

  for (const Point& point : vertices_) {
    min_x = std::min(min_x, point.x);
    max_x = std::max(max_x, point.x);
    min_y = std::min(min_y, point.y);
    max_y = std::max(max_y, point.y);
  }
  return {min_x, max_x, min_y, max_y};
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
// Builds the possible ear triangle at vertex i using active neighbor links.
// ----------------------------------------------------------------------------
void Polygon::InitializeClipping()
{
  removed_.assign(vertices_.size(), false);
  previous_.resize(vertices_.size());
  next_.resize(vertices_.size());
  for (size_t i = 0; i < vertices_.size(); ++i) {
    previous_[i] = PreviousIndex(i, vertices_.size());
    next_[i] = NextIndex(i, vertices_.size());
  }
  tree_ = std::make_unique<KDTree>(vertices_);
  remaining_vertices_ = vertices_.size();
  clipping_initialized_ = true;
}

// ----------------------------------------------------------------------------
// Returns the number of vertices that have not been clipped.
// ----------------------------------------------------------------------------
size_t Polygon::RemainingVertices() const
{
  RequireClippingInitialized();
  return remaining_vertices_;
}

// ----------------------------------------------------------------------------
// Returns true when vertex i has already been clipped.
// ----------------------------------------------------------------------------
bool Polygon::IsClipped(size_t i) const
{
  RequireClippingInitialized();
  return removed_[i];
}

// ----------------------------------------------------------------------------
// Builds a polygon containing active vertices in linked-list order.
// ----------------------------------------------------------------------------
Polygon Polygon::ActivePolygon() const
{
  RequireClippingInitialized();

  std::vector<Point> vertices;
  vertices.reserve(vertices_.size());
  size_t start = vertices_.size();
  for (size_t i = 0; i < vertices_.size(); ++i) {
    if (!removed_[i]) {
      start = i;
      break;
    }
  }

  if (start == vertices_.size()) {
    return Polygon(std::move(vertices));
  }

  size_t current = start;
  do {
    vertices.push_back(vertices_[current]);
    current = next_[current];
  } while (current != start);

  return Polygon(std::move(vertices));
}

// ----------------------------------------------------------------------------
// Builds the possible ear triangle at vertex i using active neighbor links.
// ----------------------------------------------------------------------------
Triangle Polygon::EarCandidateAt(size_t i) const
{
  RequireClippingInitialized();
  return {vertices_[previous_[i]], vertices_[i], vertices_[next_[i]]};
}

// ----------------------------------------------------------------------------
// Returns true when vertex i is a valid ear.
//
// Test 1: the ear tip must be convex. The polygon is normalized to
// counter-clockwise order before clipping. In that orientation, a convex corner
// makes a left turn, which means the cross product is positive. A negative or
// near-zero value means the corner is reflex or collinear.
//
// Test 2: no other polygon vertex may lie inside the candidate ear. If another
// vertex is inside the triangle, clipping it would cover part of the remaining
// polygon. Adjacent vertices are skipped because they are already corners of
// the candidate triangle.
// ----------------------------------------------------------------------------
bool Polygon::IsEar(size_t i) const
{
  RequireClippingInitialized();
  if (removed_[i]) {
    return false;
  }

  const Triangle ear = EarCandidateAt(i);

  // Test 1: the ear tip must be convex.
  if (ear.a.Cross(ear.b, ear.c) <= kEps) {
    return false;
  }

  // A spatial index such as a KD-tree could reduce the number of candidate
  // vertices checked here, especially for large polygons. This implementation
  // keeps the scan simple and uses the triangle's bounding box as a cheap
  // precheck before the exact point-in-triangle test.
  const Bounds bounds = ear.GetBounds();
  const std::vector<size_t> candidates = tree_->Query(bounds);
  for (size_t candidate : candidates) {
    if (candidate == i || candidate == previous_[i] || candidate == next_[i] ||
        removed_[candidate]) {
      continue;
    }
    // Test 2: no other polygon vertex may lie inside the candidate ear.
    if (ear.IsInside(vertices_[candidate])) {
      return false;
    }
  }
  return true;
}

// ----------------------------------------------------------------------------
// Marks vertex i as removed and connects its active neighbors to each other.
// ----------------------------------------------------------------------------
void Polygon::ClipVertex(size_t i)
{
  RequireClippingInitialized();
  removed_[i] = true;
  next_[previous_[i]] = next_[i];
  previous_[next_[i]] = previous_[i];
  --remaining_vertices_;
}

// ----------------------------------------------------------------------------
// Returns the final triangle formed by the three remaining active vertices.
// ----------------------------------------------------------------------------
Triangle Polygon::FinalTriangle() const
{
  RequireClippingInitialized();
  size_t start = vertices_.size();
  for (size_t i = 0; i < vertices_.size(); ++i) {
    if (!removed_[i]) {
      start = i;
      break;
    }
  }
  return {vertices_[start], vertices_[next_[start]], vertices_[next_[next_[start]]]};
}

// ----------------------------------------------------------------------------
// Clears clipping state after the polygon vertex storage changes.
// ----------------------------------------------------------------------------
void Polygon::ResetClippingState()
{
  removed_.clear();
  previous_.clear();
  next_.clear();
  tree_.reset();
  remaining_vertices_ = 0;
  clipping_initialized_ = false;
}

// ----------------------------------------------------------------------------
// Throws when clipping-only methods are used before clipping initialization.
// ----------------------------------------------------------------------------
void Polygon::RequireClippingInitialized() const
{
  if (!clipping_initialized_) {
    throw std::runtime_error("Polygon clipping state has not been initialized");
  }
}

}  // namespace geometry
