#ifndef CSV_OUTPUT_H
#define CSV_OUTPUT_H

#include "Triangle.h"

#include <iosfwd>
#include <string>
#include <vector>

namespace geometry {

void PrintResult(const std::vector<Triangle>& triangles, std::ostream& out);
void WriteTrianglesCsv(const std::vector<Triangle>& triangles, const std::string& path);

}  // namespace geometry

#endif  // CSV_OUTPUT_H
