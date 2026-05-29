#include "EarClipping.h"
#include "SvgOutput.h"

#include <filesystem>
#include <stdexcept>

namespace geometry {

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
  polygon.InitializeClipping();

  int svg_step = 0;
  if (write_svg) {
    WriteSvgStep(output_dir, svg_step++, polygon, triangles, nullptr, bounds,
                 "Initial polygon");
  }

  size_t guard = 0;
  while (polygon.RemainingVertices() > 3) {
    bool clipped = false;

    for (size_t i = 0; i < original_size; ++i) {
      if (polygon.IsClipped(i) || !polygon.IsEar(i)) continue;

      const Triangle ear = polygon.EarCandidateAt(i);
      if (write_svg) {
        const Polygon current_polygon = polygon.ActivePolygon();
        WriteSvgStep(output_dir, svg_step++, current_polygon, triangles, &ear, bounds,
                     "Clip ear at vertex " + std::to_string(i));
      }

      triangles.push_back(ear);  // Save the ear triangle before modifying the polygon.
      polygon.ClipVertex(i);     // Remove the ear tip from the polygon.

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

  const Triangle final_triangle = polygon.FinalTriangle();
  if (write_svg) {
    const Polygon current_polygon = polygon.ActivePolygon();
    WriteSvgStep(output_dir, svg_step++, current_polygon, triangles, &final_triangle, bounds,
                 "Final triangle");
  }
  triangles.push_back(final_triangle);
  return triangles;
}

}  // namespace geometry
