#include "CsvInput.h"
#include "CsvOutput.h"
#include "EarClipping.h"
#include "GeometryTypes.h"
#include "UnitTests.h"

#include <iostream>
#include <stdexcept>
#include <string>

namespace {

// ----------------------------------------------------------------------------
// Prints the supported command-line arguments.
// ----------------------------------------------------------------------------
void PrintUsage(const char* program_name)
{
  std::cerr << "Usage: " << program_name << " polygon.csv [--svg output_folder] "
            << "[--csv triangles.csv]\n"
            << "       " << program_name << " --test\n";
}

}  // namespace

// ----------------------------------------------------------------------------
// Program entry point. It handles command-line arguments, loads the input
// polygon, runs ear clipping, and optionally writes SVG and CSV output.
// ----------------------------------------------------------------------------
int main(int argc, char** argv)
{
  try {
    if (argc == 2 && std::string(argv[1]) == "--test") {
      geometry::RunTests();
      return 0;
    }
    if (argc < 2) {
      PrintUsage(argv[0]);
      return 1;
    }

    const std::string input_path = argv[1];
    std::string svg_output_dir;
    std::string csv_output_path;

    for (int i = 2; i < argc; ++i) {
      const std::string option = argv[i];
      if (option == "--svg" && i + 1 < argc) {
        svg_output_dir = argv[++i];
      } else if (option == "--csv" && i + 1 < argc) {
        csv_output_path = argv[++i];
      } else {
        PrintUsage(argv[0]);
        return 1;
      }
    }

    const auto polygon = geometry::LoadCsvPolygon(input_path);
    const auto triangles = geometry::TriangulateEarClipping(polygon, svg_output_dir);
    geometry::PrintResult(triangles, std::cout);

    if (!csv_output_path.empty()) {
      geometry::WriteTrianglesCsv(triangles, csv_output_path);
      std::cout << "Triangle CSV written to: " << csv_output_path << '\n';
    }
    if (!svg_output_dir.empty()) {
      std::cout << "SVG ear clipping steps written to: " << svg_output_dir << '\n';
    }
    return 0;
  } catch (const std::exception& e) {
    std::cerr << "error: " << e.what() << '\n';
    return 1;
  }
}
