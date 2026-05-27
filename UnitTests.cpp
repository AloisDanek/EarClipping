#include "UnitTests.h"

#include "CsvInput.h"
#include "EarClipping.h"
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
  ExpectNear(Cross({0, 0}, {1, 0}, {1, 1}), 1.0, "left turn cross");
  ExpectNear(Cross({0, 0}, {1, 0}, {1, -1}), -1.0, "right turn cross");
  ExpectNear(Cross({0, 0}, {1, 1}, {2, 2}), 0.0, "collinear cross");

  ExpectTrue(NearlyEqual({1.0, 2.0}, {1.0 + 0.5e-12, 2.0 - 0.5e-12}),
             "nearly equal points");
  ExpectTrue(!NearlyEqual({1.0, 2.0}, {1.0 + 1.0e-9, 2.0}),
             "different points");
}

// ----------------------------------------------------------------------------
// Verifies triangle area helpers.
// ----------------------------------------------------------------------------
void TestTriangleHelpers()
{
  const Triangle triangle{{0, 0}, {4, 0}, {0, 3}};
  const Triangle reversed{{0, 3}, {4, 0}, {0, 0}};

  ExpectNear(TriangleArea(triangle), 6.0, "triangle area helper");
  ExpectNear(TriangleArea(reversed), 6.0, "reversed triangle area helper");
  ExpectNear(SumTriangleAreas({triangle, reversed}), 12.0,
             "sum triangle areas helper");
}

// ----------------------------------------------------------------------------
// Verifies polygon storage, navigation, signed area, and ear helpers.
// ----------------------------------------------------------------------------
void TestPolygonHelpers()
{
  Polygon square{{0, 0}, {1, 0}, {1, 1}, {0, 1}};

  ExpectTrue(square.size() == 4, "polygon size");
  ExpectTrue(!square.empty(), "polygon non-empty");
  ExpectTrue(NearlyEqual(square.front(), {0, 0}), "polygon front");
  ExpectTrue(NearlyEqual(square.back(), {0, 1}), "polygon back");
  ExpectTrue(NearlyEqual(square.PreviousVertex(0), {0, 1}), "previous vertex");
  ExpectTrue(NearlyEqual(square.NextVertex(3), {0, 0}), "next vertex");
  ExpectNear(square.SignedArea(), 1.0, "polygon signed area");
  ExpectTrue(square.IsEar(0), "square ear");

  const Triangle ear = square.EarCandidateAt(0);
  ExpectTrue(NearlyEqual(ear.a, {0, 1}), "ear previous point");
  ExpectTrue(NearlyEqual(ear.b, {0, 0}), "ear current point");
  ExpectTrue(NearlyEqual(ear.c, {1, 0}), "ear next point");

  square.reverse();
  ExpectNear(square.SignedArea(), -1.0, "reversed polygon signed area");

  Polygon concave{{0, 0}, {2, 0}, {2, 1}, {1, 0.5}, {0, 1}};
  ExpectTrue(!concave.IsEar(3), "reflex vertex is not an ear");
  concave.erase(3);
  ExpectTrue(concave.size() == 4, "polygon erase");
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
  TestTriangulation();
  TestCsvInput();
  TestInvalidPolygons();

  std::cout << "All tests passed.\n";
}

}  // namespace geometry
