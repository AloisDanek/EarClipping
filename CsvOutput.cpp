#include "CsvOutput.h"

#include <fstream>
#include <iomanip>
#include <ostream>
#include <stdexcept>

namespace geometry {

// ----------------------------------------------------------------------------
// Prints each triangle and the total triangulated area to an output stream.
// ----------------------------------------------------------------------------
void PrintResult(const std::vector<Triangle>& triangles, std::ostream& out)
{
  out << std::fixed << std::setprecision(15);
  double total = 0.0;
  for (size_t i = 0; i < triangles.size(); ++i) {
    const Triangle& t = triangles[i];
    const double area = TriangleArea(t);
    total += area;
    out << "triangle " << (i + 1) << ": " << t.a << ", " << t.b << ", "
        << t.c << "  area=" << area << '\n';
  }
  out << "total area=" << total << '\n';
}

// ----------------------------------------------------------------------------
// Writes the triangulation to CSV with one row per triangle.
// ----------------------------------------------------------------------------
void WriteTrianglesCsv(const std::vector<Triangle>& triangles, const std::string& path)
{
  std::ofstream out(path);
  if (!out) {
    throw std::runtime_error("Could not write CSV file: " + path);
  }

  out << std::fixed << std::setprecision(15);
  out << "triangle,ax,ay,bx,by,cx,cy,area\n";
  for (size_t i = 0; i < triangles.size(); ++i) {
    const Triangle& t = triangles[i];
    out << (i + 1) << ','
        << t.a.x << ',' << t.a.y << ','
        << t.b.x << ',' << t.b.y << ','
        << t.c.x << ',' << t.c.y << ','
        << TriangleArea(t) << '\n';
  }
}

}  // namespace geometry
