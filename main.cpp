#include "CsvInput.h"
#include "CsvOutput.h"
#include "EarClipping.h"
#include "GeometryTypes.h"

#include <cmath>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

namespace {

// ----------------------------------------------------------------------------
// Throws if two floating-point values differ by more than the test tolerance.
// ----------------------------------------------------------------------------
void ExpectNear(double actual, double expected, const std::string& name)
{
  const double tolerance = 1e-9;
  if (std::abs(actual - expected) > tolerance) {
    std::ostringstream msg;
    msg << name << " failed: expected " << expected << ", got " << actual;
    throw std::runtime_error(msg.str());
  }
}

// ----------------------------------------------------------------------------
// Throws if the supplied function does not throw an exception.
// ----------------------------------------------------------------------------
template <typename Function>
void ExpectThrows(Function function, const std::string& name)
{
  try {
    function();
  } catch (const std::exception&) {
    return;
  }
  throw std::runtime_error(name + " failed: expected an exception");
}

// ----------------------------------------------------------------------------
// Runs correctness checks for normal cases and important corner cases.
// ----------------------------------------------------------------------------
void RunTests()
{
  {
    const geometry::Polygon triangle{{0, 0}, {4, 0}, {0, 3}};
    const auto triangles = geometry::TriangulateEarClipping(triangle);
    if (triangles.size() != 1) throw std::runtime_error("triangle count failed");
    ExpectNear(geometry::SumTriangleAreas(triangles), 6.0, "triangle area");
  }
  {
    const geometry::Polygon square{{0, 0}, {1, 0}, {1, 1}, {0, 1}};
    const auto triangles = geometry::TriangulateEarClipping(square);
    if (triangles.size() != 2) throw std::runtime_error("square triangle count failed");
    ExpectNear(geometry::SumTriangleAreas(triangles), 1.0, "square area");
  }
  {
    const geometry::Polygon concave{{0, 0}, {2, 0}, {2, 1}, {1, 0.5}, {0, 1}};
    const auto triangles = geometry::TriangulateEarClipping(concave);
    if (triangles.size() != 3) throw std::runtime_error("concave triangle count failed");
    ExpectNear(geometry::SumTriangleAreas(triangles), 1.5, "concave area");
  }
  {
    const geometry::Polygon clockwise_square{{0, 0}, {0, 1}, {1, 1}, {1, 0}};
    const auto triangles = geometry::TriangulateEarClipping(clockwise_square);
    if (triangles.size() != 2) {
      throw std::runtime_error("clockwise square triangle count failed");
    }
    ExpectNear(geometry::SumTriangleAreas(triangles), 1.0, "clockwise square area");
  }
  {
    const geometry::Polygon clockwise_concave{{0, 1}, {1, 0.5}, {2, 1}, {2, 0}, {0, 0}};
    const auto triangles = geometry::TriangulateEarClipping(clockwise_concave);
    if (triangles.size() != 3) {
      throw std::runtime_error("clockwise concave triangle count failed");
    }
    ExpectNear(geometry::SumTriangleAreas(triangles), 1.5, "clockwise concave area");
  }
  {
    const geometry::Polygon negative_coordinates{{-2, -1}, {2, -1}, {2, 1}, {-2, 1}};
    const auto triangles = geometry::TriangulateEarClipping(negative_coordinates);
    if (triangles.size() != 2) {
      throw std::runtime_error("negative coordinates triangle count failed");
    }
    ExpectNear(geometry::SumTriangleAreas(triangles), 8.0, "negative coordinates area");
  }
  {
    const char* test_path = "test_closed_polygon.csv";
    {
      std::ofstream csv(test_path);
      csv << "x;y\n"
          << "0;0\n"
          << "2;0\n"
          << "2;1\n"
          << "0;1\n"
          << "0;0\n";
    }

    const geometry::Polygon loaded = geometry::LoadCsvPolygon(test_path);
    std::remove(test_path);
    if (loaded.size() != 4) {
      throw std::runtime_error("closed CSV polygon did not remove repeated final vertex");
    }
    const auto triangles = geometry::TriangulateEarClipping(loaded);
    ExpectNear(geometry::SumTriangleAreas(triangles), 2.0, "closed CSV polygon area");
  }
  {
    ExpectThrows(
        [] {
          geometry::TriangulateEarClipping(geometry::Polygon{{0, 0}, {1, 0}});
        },
        "too few vertices");
  }
  {
    ExpectThrows(
        [] {
          geometry::TriangulateEarClipping(
              geometry::Polygon{{0, 0}, {1, 0}, {2, 0}, {3, 0}});
        },
        "zero area polygon");
  }
  {
    const geometry::Polygon duplicate_vertex{{0, 0}, {1, 0}, {1, 0}, {1, 1}, {0, 1}};
    ExpectThrows(
        [&duplicate_vertex] {
          geometry::TriangulateEarClipping(duplicate_vertex);
        },
        "duplicate vertex polygon");
  }
  std::cout << "All tests passed.\n";
}

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
      RunTests();
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
