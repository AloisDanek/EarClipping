#ifndef GEOMETRY_TYPES_H
#define GEOMETRY_TYPES_H

#include <cstddef>

namespace geometry {

constexpr double kEps = 1.0e-12;

class Polygon;

size_t PreviousIndex(size_t i, size_t n);
size_t NextIndex(size_t i, size_t n);
double PolygonSignedArea(const Polygon& poly);

}  // namespace geometry

#endif  // GEOMETRY_TYPES_H
