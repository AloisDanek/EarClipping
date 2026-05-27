#include "SvgOutput.h"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace geometry {
namespace {

// ----------------------------------------------------------------------------
// Converts mathematical coordinates to SVG coordinates. SVG's y-axis points
// downward, so the y value is flipped when mapped into the 800x800 canvas.
// ----------------------------------------------------------------------------
Point SvgPoint(const Point& p, const SvgBounds& bounds)
{
  constexpr double kSize = 800.0;
  constexpr double kMargin = 60.0;
  const double width = bounds.max_x - bounds.min_x;
  const double height = bounds.max_y - bounds.min_y;
  const double scale = (kSize - 2.0 * kMargin) / std::max(width, height);
  const double x_offset = (kSize - width * scale) * 0.5;
  const double y_offset = (kSize - height * scale) * 0.5;
  return {x_offset + (p.x - bounds.min_x) * scale,
          kSize - (y_offset + (p.y - bounds.min_y) * scale)};
}

// ----------------------------------------------------------------------------
// Formats a polygon as an SVG point list: "x1,y1 x2,y2 x3,y3".
// ----------------------------------------------------------------------------
std::string SvgPoints(const Polygon& polygon, const SvgBounds& bounds)
{
  std::ostringstream points;
  points << std::fixed << std::setprecision(3);
  for (const Point& p : polygon) {
    const Point q = SvgPoint(p, bounds);
    points << q.x << ',' << q.y << ' ';
  }
  return points.str();
}

// ----------------------------------------------------------------------------
// Formats one triangle as an SVG point list by reusing the polygon formatter.
// ----------------------------------------------------------------------------
std::string SvgTrianglePoints(const Triangle& t, const SvgBounds& bounds)
{
  return SvgPoints(Polygon{t.a, t.b, t.c}, bounds);
}

}  // namespace

// ----------------------------------------------------------------------------
// Computes the original polygon bounds used by SVG output. Every generated SVG
// step uses these same bounds, so the visualization does not jump or rescale.
// ----------------------------------------------------------------------------
SvgBounds ComputeSvgBounds(const Polygon& polygon)
{
  SvgBounds b;
  b.min_x = b.max_x = polygon.front().x;
  b.min_y = b.max_y = polygon.front().y;
  for (const Point& p : polygon) {
    b.min_x = std::min(b.min_x, p.x);
    b.max_x = std::max(b.max_x, p.x);
    b.min_y = std::min(b.min_y, p.y);
    b.max_y = std::max(b.max_y, p.y);
  }
  if (std::abs(b.max_x - b.min_x) <= kEps) b.max_x = b.min_x + 1.0;
  if (std::abs(b.max_y - b.min_y) <= kEps) b.max_y = b.min_y + 1.0;
  return b;
}

// ----------------------------------------------------------------------------
// Writes one SVG frame of the algorithm. Already clipped triangles are light
// blue, the current ear is orange, and the remaining polygon boundary is black.
// ----------------------------------------------------------------------------
void WriteSvgStep(const std::filesystem::path& output_dir,
                  int step,
                  const Polygon& polygon,
                  const std::vector<Triangle>& clipped_triangles,
                  const Triangle* current_ear,
                  const SvgBounds& bounds,
                  const std::string& title)
{
  std::ostringstream name;
  name << "step_" << std::setw(3) << std::setfill('0') << step << ".svg";
  const std::filesystem::path path = output_dir / name.str();

  std::ofstream out(path);
  if (!out) {
    throw std::runtime_error("Could not write SVG file: " + path.string());
  }

  out << "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"800\" height=\"800\" "
      << "viewBox=\"0 0 800 800\">\n";
  out << "<rect width=\"800\" height=\"800\" fill=\"white\"/>\n";
  out << "<text x=\"20\" y=\"35\" font-family=\"Arial\" font-size=\"22\">"
      << title << "</text>\n";

  for (const Triangle& t : clipped_triangles) {
    out << "<polygon points=\"" << SvgTrianglePoints(t, bounds)
        << "\" fill=\"#bcd7ff\" stroke=\"#4878b8\" stroke-width=\"1\" "
        << "fill-opacity=\"0.35\"/>\n";
  }

  if (current_ear != nullptr) {
    out << "<polygon points=\"" << SvgTrianglePoints(*current_ear, bounds)
        << "\" fill=\"#ffcc66\" stroke=\"#cc7a00\" stroke-width=\"4\" "
        << "fill-opacity=\"0.70\"/>\n";
  }

  out << "<polygon points=\"" << SvgPoints(polygon, bounds)
      << "\" fill=\"none\" stroke=\"black\" stroke-width=\"3\"/>\n";

  for (size_t i = 0; i < polygon.size(); ++i) {
    const Point q = SvgPoint(polygon[i], bounds);
    out << "<circle cx=\"" << q.x << "\" cy=\"" << q.y
        << "\" r=\"5\" fill=\"black\"/>\n";
    out << "<text x=\"" << (q.x + 8.0) << "\" y=\"" << (q.y - 8.0)
        << "\" font-family=\"Arial\" font-size=\"14\">" << i << "</text>\n";
  }

  out << "</svg>\n";
}

}  // namespace geometry
