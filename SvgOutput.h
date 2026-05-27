#ifndef SVG_OUTPUT_H
#define SVG_OUTPUT_H

#include "Polygon.h"

#include <filesystem>
#include <string>
#include <vector>

namespace geometry {

struct SvgBounds {
  double min_x = 0.0;
  double max_x = 1.0;
  double min_y = 0.0;
  double max_y = 1.0;
};

SvgBounds ComputeSvgBounds(const Polygon& polygon);

void WriteSvgStep(const std::filesystem::path& output_dir,
                  int step,
                  const Polygon& polygon,
                  const std::vector<Triangle>& clipped_triangles,
                  const Triangle* current_ear,
                  const SvgBounds& bounds,
                  const std::string& title);

}  // namespace geometry

#endif  // SVG_OUTPUT_H
