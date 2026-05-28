#include "UnitTests.h"

#include "Bounds.h"
#include "CsvInput.h"
#include "EarClipping.h"
#include "KDTree.h"
#include "Point.h"
#include "Polygon.h"
#include "Triangle.h"

#include <cmath>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

namespace geometry {
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
// Throws if a condition is false.
// ----------------------------------------------------------------------------
void ExpectTrue(bool condition, const std::string& name)
{
  if (!condition) {
    throw std::runtime_error(name + " failed");
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
// Verifies point comparison and orientation helpers.
// ----------------------------------------------------------------------------
void TestPointHelpers()
{
  ExpectNear(Point{0, 0}.Cross({1, 0}, {1, 1}), 1.0, "left turn cross");
  ExpectNear(Point{0, 0}.Cross({1, 0}, {1, -1}), -1.0, "right turn cross");
  ExpectNear(Point{0, 0}.Cross({1, 1}, {2, 2}), 0.0, "collinear cross");

  ExpectTrue(Point{1.0, 2.0}.NearlyEquals({1.0 + 0.5e-12, 2.0 - 0.5e-12}),
             "nearly equal points");
  ExpectTrue(!Point{1.0, 2.0}.NearlyEquals({1.0 + 1.0e-9, 2.0}),
             "different points");
}

// ----------------------------------------------------------------------------
// Verifies triangle area helpers.
// ----------------------------------------------------------------------------
void TestTriangleHelpers()
{
  const Triangle triangle{{0, 0}, {4, 0}, {0, 3}};
  const Triangle reversed{{0, 3}, {4, 0}, {0, 0}};

  ExpectNear(triangle.Area(), 6.0, "triangle area helper");
  ExpectNear(reversed.Area(), 6.0, "reversed triangle area helper");
  ExpectNear(SumTriangleAreas({triangle, reversed}), 12.0,
             "sum triangle areas helper");
}

// ----------------------------------------------------------------------------
// Verifies polygon storage, navigation, signed area, and ear helpers.
// ----------------------------------------------------------------------------
void TestPolygonHelpers()
{
  Polygon square{{0, 0}, {1, 0}, {1, 1}, {0, 1}};
  std::vector<char> square_removed(square.size(), false);
  std::vector<size_t> square_previous{3, 0, 1, 2};
  std::vector<size_t> square_next{1, 2, 3, 0};
  const KDTree square_tree(square);

  ExpectTrue(square.size() == 4, "polygon size");
  ExpectTrue(!square.empty(), "polygon non-empty");
  ExpectTrue(square.front().NearlyEquals({0, 0}), "polygon front");
  ExpectTrue(square.back().NearlyEquals({0, 1}), "polygon back");
  ExpectTrue(square.PreviousVertex(0).NearlyEquals({0, 1}), "previous vertex");
  ExpectTrue(square.NextVertex(3).NearlyEquals({0, 0}), "next vertex");
  ExpectNear(square.SignedArea(), 1.0, "polygon signed area");
  ExpectTrue(square.IsEar(square_removed, square_previous, square_next, square_tree, 0),
             "square ear");
  const Bounds square_bounds = square.GetBounds();
  ExpectTrue(square_bounds.Contains({0, 0}), "polygon bounds contains minimum");
  ExpectTrue(square_bounds.Contains({1, 1}), "polygon bounds contains maximum");
  ExpectTrue(!square_bounds.Contains({2, 2}), "polygon bounds excludes outside point");

  const Triangle ear = square.EarCandidateAt(square_previous, square_next, 0);
  ExpectTrue(ear.a.NearlyEquals({0, 1}), "ear previous point");
  ExpectTrue(ear.b.NearlyEquals({0, 0}), "ear current point");
  ExpectTrue(ear.c.NearlyEquals({1, 0}), "ear next point");

  square.reverse();
  ExpectNear(square.SignedArea(), -1.0, "reversed polygon signed area");

  Polygon concave{{0, 0}, {2, 0}, {2, 1}, {1, 0.5}, {0, 1}};
  std::vector<char> concave_removed(concave.size(), false);
  std::vector<size_t> concave_previous{4, 0, 1, 2, 3};
  std::vector<size_t> concave_next{1, 2, 3, 4, 0};
  const KDTree concave_tree(concave);
  ExpectTrue(!concave.IsEar(concave_removed, concave_previous, concave_next,
                            concave_tree, 3),
             "reflex vertex is not an ear");
  concave.erase(3);
  ExpectTrue(concave.size() == 4, "polygon erase");
}

// ----------------------------------------------------------------------------
// Verifies KD-tree rectangle queries over stable polygon vertex indices.
// ----------------------------------------------------------------------------
void TestKDTreeQueries()
{
  const Polygon polygon{{0, 0}, {2, 0}, {2, 2}, {1, 1}, {0, 2}};
  const KDTree tree(polygon);
  const std::vector<size_t> candidates = tree.Query({0.5, 2.0, 0.5, 2.0});

  ExpectTrue(candidates.size() == 2, "KD-tree candidate count");
  ExpectTrue(candidates[0] == 2 || candidates[1] == 2,
             "KD-tree includes upper right point");
  ExpectTrue(candidates[0] == 3 || candidates[1] == 3,
             "KD-tree includes center point");
}

// ----------------------------------------------------------------------------
// Verifies ear clipping output for normal polygons.
// ----------------------------------------------------------------------------
void TestTriangulation()
{
  {
    const Polygon triangle{{0, 0}, {4, 0}, {0, 3}};
    const auto triangles = TriangulateEarClipping(triangle);
    ExpectTrue(triangles.size() == 1, "triangle count");
    ExpectNear(SumTriangleAreas(triangles), 6.0, "triangle area");
  }
  {
    const Polygon square{{0, 0}, {1, 0}, {1, 1}, {0, 1}};
    const auto triangles = TriangulateEarClipping(square);
    ExpectTrue(triangles.size() == 2, "square triangle count");
    ExpectNear(SumTriangleAreas(triangles), 1.0, "square area");
  }
  {
    const Polygon concave{{0, 0}, {2, 0}, {2, 1}, {1, 0.5}, {0, 1}};
    const auto triangles = TriangulateEarClipping(concave);
    ExpectTrue(triangles.size() == 3, "concave triangle count");
    ExpectNear(SumTriangleAreas(triangles), 1.5, "concave area");
  }
  {
    const Polygon clockwise_square{{0, 0}, {0, 1}, {1, 1}, {1, 0}};
    const auto triangles = TriangulateEarClipping(clockwise_square);
    ExpectTrue(triangles.size() == 2, "clockwise square triangle count");
    ExpectNear(SumTriangleAreas(triangles), 1.0, "clockwise square area");
  }
  {
    const Polygon clockwise_concave{{0, 1}, {1, 0.5}, {2, 1}, {2, 0}, {0, 0}};
    const auto triangles = TriangulateEarClipping(clockwise_concave);
    ExpectTrue(triangles.size() == 3, "clockwise concave triangle count");
    ExpectNear(SumTriangleAreas(triangles), 1.5, "clockwise concave area");
  }
  {
    const Polygon negative_coordinates{{-2, -1}, {2, -1}, {2, 1}, {-2, 1}};
    const auto triangles = TriangulateEarClipping(negative_coordinates);
    ExpectTrue(triangles.size() == 2, "negative coordinates triangle count");
    ExpectNear(SumTriangleAreas(triangles), 8.0, "negative coordinates area");
  }
}

// ----------------------------------------------------------------------------
// Verifies CSV loading behavior.
// ----------------------------------------------------------------------------
void TestCsvInput()
{
  const char* test_path = "test_closed_polygon.csv";
  {
    std::ofstream csv(test_path);
    csv << "x;y\n"
        << "0;0\n"
        << "ignored,row\n"
        << "2;0\n"
        << "2;1\n"
        << "0;1\n"
        << "0;0\n";
  }

  const Polygon loaded = LoadCsvPolygon(test_path);
  std::remove(test_path);
  ExpectTrue(loaded.size() == 4,
             "closed CSV polygon did not remove repeated final vertex");
  const auto triangles = TriangulateEarClipping(loaded);
  ExpectNear(SumTriangleAreas(triangles), 2.0, "closed CSV polygon area");
}

// ----------------------------------------------------------------------------
// Verifies expected failures for invalid polygon input.
// ----------------------------------------------------------------------------
void TestInvalidPolygons()
{
  ExpectThrows(
      [] {
        TriangulateEarClipping(Polygon{{0, 0}, {1, 0}});
      },
      "too few vertices");

  ExpectThrows(
      [] {
        TriangulateEarClipping(Polygon{{0, 0}, {1, 0}, {2, 0}, {3, 0}});
      },
      "zero area polygon");

  const Polygon duplicate_vertex{{0, 0}, {1, 0}, {1, 0}, {1, 1}, {0, 1}};
  ExpectThrows(
      [&duplicate_vertex] {
        TriangulateEarClipping(duplicate_vertex);
      },
      "duplicate vertex polygon");
}

}  // namespace

// ----------------------------------------------------------------------------
// Runs correctness checks for normal cases and important corner cases.
// ----------------------------------------------------------------------------
void RunTests()
{
  TestPointHelpers();
  TestTriangleHelpers();
  TestPolygonHelpers();
  TestKDTreeQueries();
  TestTriangulation();
  TestCsvInput();
  TestInvalidPolygons();

  std::cout << "All tests passed.\n";
}

}  // namespace geometry
