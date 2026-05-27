#ifndef CSV_INPUT_H
#define CSV_INPUT_H

#include "GeometryTypes.h"

#include <string>

namespace geometry {

Polygon LoadCsvPolygon(const std::string& path);

}  // namespace geometry

#endif  // CSV_INPUT_H
