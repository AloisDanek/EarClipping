#include "EarClipping.h"
#include "KDTree.h"
#include "SvgOutput.h"

#include <filesystem>
#include <stdexcept>
#include <utility>
#include <vector>

namespace geometry {
namespace {

// ----------------------------------------------------------------------------
// Builds a polygon containing only vertices that have not been clipped yet.
// ----------------------------------------------------------------------------
Polygon ActivePolygon(const Polygon& polygon,
                      const std::vector<char>& removed,
                      const std::vector<size_t>& next)
{
  std::vector<Point> vertices;
  vertices.reserve(polygon.size());
  size_t start = polygon.size();
  for (size_t i = 0; i < polygon.size(); ++i) {
    if (!removed[i]) {
      start = i;
      break;
    }
  }

  if (start == polygon.size()) {
    return Polygon(std::move(vertices));
  }

  size_t current = start;
  do {
    vertices.push_back(polygon[current]);
    current = next[current];
  } while (current != start);

  return Polygon(std::move(vertices));
}

// ----------------------------------------------------------------------------
// Marks vertex i as removed and connects its active neighbors to each other.
// ----------------------------------------------------------------------------
void ClipVertex(std::vector<char>& removed,
                std::vector<size_t>& previous,
                std::vector<size_t>& next,
                size_t i)
{
  removed[i] = true;
  next[previous[i]] = next[i];
  previous[next[i]] = previous[i];
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

  if (polygon.SignedArea() < 0.0) {
    polygon.reverse();
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

  const size_t original_size = polygon.size();
  std::vector<char> removed(original_size, false);
  std::vector<size_t> previous(original_size);
  std::vector<size_t> next(original_size);
  for (size_t i = 0; i < original_size; ++i) {
    previous[i] = PreviousIndex(i, original_size);
    next[i] = NextIndex(i, original_size);
  }
  const KDTree tree(polygon);

  int svg_step = 0;
  if (write_svg) {
    WriteSvgStep(output_dir, svg_step++, polygon, triangles, nullptr, bounds,
                 "Initial polygon");
  }

  size_t remaining_vertices = polygon.size();
  size_t guard = 0;
  while (remaining_vertices > 3) {
    bool clipped = false;

    for (size_t i = 0; i < original_size; ++i) {
      if (!polygon.IsEar(removed, previous, next, tree, i)) continue;

      const Triangle ear = polygon.EarCandidateAt(previous, next, i);
      if (write_svg) {
        const Polygon current_polygon = ActivePolygon(polygon, removed, next);
        WriteSvgStep(output_dir, svg_step++, current_polygon, triangles, &ear, bounds,
                     "Clip ear at vertex " + std::to_string(i));
      }

      triangles.push_back(ear);
      ClipVertex(removed, previous, next, i);
      --remaining_vertices;
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

  size_t final_start = original_size;
  for (size_t i = 0; i < original_size; ++i) {
    if (!removed[i]) {
      final_start = i;
      break;
    }
  }
  const Triangle final_triangle{
      polygon[final_start], polygon[next[final_start]], polygon[next[next[final_start]]]};
  if (write_svg) {
    const Polygon current_polygon = ActivePolygon(polygon, removed, next);
    WriteSvgStep(output_dir, svg_step++, current_polygon, triangles, &final_triangle, bounds,
                 "Final triangle");
  }
  triangles.push_back(final_triangle);
  return triangles;
}

}  // namespace geometry
