#include "EarClipping.h"
#include "SvgOutput.h"

#include <algorithm>
#include <filesystem>
#include <stdexcept>

namespace geometry {
namespace {

// ----------------------------------------------------------------------------
// Returns the vertex before index i, wrapping from vertex 0 to the last vertex.
// ----------------------------------------------------------------------------
const Point& PreviousVertex(const Polygon& polygon, size_t i)
{
  return polygon[PreviousIndex(i, polygon.size())];
}

// ----------------------------------------------------------------------------
// Returns the vertex after index i, wrapping from the last vertex to vertex 0.
// ----------------------------------------------------------------------------
const Point& NextVertex(const Polygon& polygon, size_t i)
{
  return polygon[NextIndex(i, polygon.size())];
}

// ----------------------------------------------------------------------------
// For a counter-clockwise triangle, a point is inside or on the boundary
// when it is always on the left side of each directed triangle edge.
// Cross products close to zero are treated as zero to reduce floating-point
// roundoff problems.
// ----------------------------------------------------------------------------
bool PointInTriangle(const Point& p, const Triangle& t)
{
  const double c1 = Cross(t.a, t.b, p);
  const double c2 = Cross(t.b, t.c, p);
  const double c3 = Cross(t.c, t.a, p);
  return c1 >= -kEps && c2 >= -kEps && c3 >= -kEps;
}

// ----------------------------------------------------------------------------
// Builds the possible ear triangle at vertex i using the previous, current,
// and next vertices of the current polygon.
// ----------------------------------------------------------------------------
Triangle EarCandidateAt(const Polygon& polygon, size_t i)
{
  return {PreviousVertex(polygon, i), polygon[i], NextVertex(polygon, i)};
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
bool IsEar(const Polygon& poly, size_t i)
{
  const size_t n = poly.size();
  const Triangle ear = EarCandidateAt(poly, i);

  // Check if the ear tip is convex by testing the cross product sign. A non-positive
  // value means the ear tip is reflex or collinear, so it cannot be an ear
  if (Cross(ear.a, ear.b, ear.c) <= kEps) {
    return false;
  }

  for (size_t j = 0; j < n; ++j) {
    if (j == i || j == PreviousIndex(i, n) || j == NextIndex(i, n)) continue;
    if (PointInTriangle(poly[j], ear)) {
      return false;
    }
  }
  return true;
}

}  // namespace

// ----------------------------------------------------------------------------
// Triangulates a simple polygon using the ear clipping algorithm.
//
// The polygon is first normalized to counter-clockwise order. The algorithm
// repeatedly finds any valid ear, saves that ear as an output triangle, and
// removes only the ear tip from the working polygon. Once three vertices
// remain, those vertices form the final triangle.
//
// A valid simple polygon with n vertices always produces n - 2 triangles.
// ----------------------------------------------------------------------------
std::vector<Triangle> TriangulateEarClipping(Polygon polygon,
                                             const std::string& svg_output_dir)
{
  if (polygon.size() < 3) {
    throw std::runtime_error("Polygon must contain at least 3 vertices");
  }

  if (PolygonSignedArea(polygon) < 0.0) {
    std::reverse(polygon.begin(), polygon.end());
  }

  const bool write_svg = !svg_output_dir.empty();
  std::filesystem::path output_dir;
  SvgBounds bounds;
  if (write_svg) {
    output_dir = svg_output_dir;
    std::filesystem::create_directories(output_dir);
    bounds = ComputeSvgBounds(polygon);
  }

  std::vector<Triangle> triangles;
  triangles.reserve(polygon.size() - 2);

  int svg_step = 0;
  if (write_svg) {
    WriteSvgStep(output_dir, svg_step++, polygon, triangles, nullptr, bounds,
                 "Initial polygon");
  }

  size_t guard = 0;
  while (polygon.size() > 3) {
    bool clipped = false;
    const size_t n = polygon.size();

    for (size_t i = 0; i < n; ++i) {
      if (!IsEar(polygon, i)) continue;

      const Triangle ear = EarCandidateAt(polygon, i);
      if (write_svg) {
        WriteSvgStep(output_dir, svg_step++, polygon, triangles, &ear, bounds,
                     "Clip ear at vertex " + std::to_string(i));
      }

      triangles.push_back(ear);
      polygon.erase(polygon.begin() + static_cast<std::ptrdiff_t>(i));
      clipped = true;
      break;
    }

    if (!clipped) {
      throw std::runtime_error(
          "No ear found. Input may be self-intersecting, duplicated, or badly collinear.");
    }
    if (++guard > 1000000) {
      throw std::runtime_error("Internal guard limit reached");
    }
  }

  const Triangle final_triangle{polygon[0], polygon[1], polygon[2]};
  if (write_svg) {
    WriteSvgStep(output_dir, svg_step++, polygon, triangles, &final_triangle, bounds,
                 "Final triangle");
  }
  triangles.push_back(final_triangle);
  return triangles;
}

}  // namespace geometry
