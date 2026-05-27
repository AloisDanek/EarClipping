#ifndef EAR_CLIPPING_H
#define EAR_CLIPPING_H

#include "GeometryTypes.h"

#include <string>
#include <vector>

namespace geometry {

std::vector<Triangle> TriangulateEarClipping(
    Polygon polygon,
    const std::string& svg_output_dir = "");

}  // namespace geometry

#endif  // EAR_CLIPPING_H
