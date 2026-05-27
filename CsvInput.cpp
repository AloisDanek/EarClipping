#include "CsvInput.h"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace geometry {

// ----------------------------------------------------------------------------
// Reads polygon vertices from a CSV file. The expected format is one vertex per
// row as x,y. A header row such as "x,y" is ignored, malformed rows are
// skipped, semicolons are accepted as separators, and a repeated final vertex
// is removed.
// ----------------------------------------------------------------------------
Polygon LoadCsvPolygon(const std::string& path)
{
  std::ifstream input(path);
  if (!input) {
    throw std::runtime_error("Could not open input file: " + path);
  }

  Polygon polygon;
  std::string line;
  while (std::getline(input, line)) {
    if (line.empty()) continue;
    std::replace(line.begin(), line.end(), ';', ',');

    std::stringstream ss(line);
    std::string x_text;
    std::string y_text;
    if (!std::getline(ss, x_text, ',') || !std::getline(ss, y_text, ',')) {
      continue;  // Ignore blank/malformed lines such as comments.
    }

    try {
      size_t used_x = 0;
      size_t used_y = 0;
      const double x = std::stod(x_text, &used_x);
      const double y = std::stod(y_text, &used_y);
      (void)used_x;
      (void)used_y;
      polygon.push_back({x, y});
    } catch (const std::exception&) {
    }
  }

  if (polygon.size() >= 2 && polygon.front().NearlyEquals(polygon.back())) {
    polygon.pop_back();  // Accept either open or explicitly closed CSV input.
  }
  if (polygon.size() < 3) {
    throw std::runtime_error("Polygon must contain at least 3 vertices");
  }
  if (std::abs(polygon.SignedArea()) <= kEps) {
    throw std::runtime_error("Polygon area is zero or numerically unstable");
  }
  return polygon;
}

}  // namespace geometry
