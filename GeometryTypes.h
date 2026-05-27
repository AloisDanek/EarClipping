#ifndef GEOMETRY_TYPES_H
#define GEOMETRY_TYPES_H

#include <cstddef>
#include <iosfwd>
#include <initializer_list>
#include <vector>

namespace geometry {

constexpr double kEps = 1.0e-12;

struct Point {
  double x = 0.0;
  double y = 0.0;
};

struct Triangle {
  Point a;
  Point b;
  Point c;
};

class Polygon {
 public:
  Polygon() = default;
  Polygon(std::initializer_list<Point> vertices);
  explicit Polygon(std::vector<Point> vertices);

  size_t size() const;
  bool empty() const;

  const Point& operator[](size_t i) const;
  Point& operator[](size_t i);
  const Point& front() const;
  const Point& back() const;

  std::vector<Point>::const_iterator begin() const;
  std::vector<Point>::const_iterator end() const;

  void push_back(const Point& point);
  void pop_back();
  void erase(size_t i);
  void reverse();

  double SignedArea() const;
  const Point& PreviousVertex(size_t i) const;
  const Point& NextVertex(size_t i) const;
  Triangle EarCandidateAt(size_t i) const;
  bool IsEar(size_t i) const;

 private:
  std::vector<Point> vertices_;
};

std::ostream& operator<<(std::ostream& os, const Point& p);

size_t PreviousIndex(size_t i, size_t n);
size_t NextIndex(size_t i, size_t n);
double Cross(const Point& a, const Point& b, const Point& c);
double PolygonSignedArea(const Polygon& poly);
double TriangleArea(const Triangle& t);
double SumTriangleAreas(const std::vector<Triangle>& triangles);
bool NearlyEqual(const Point& a, const Point& b);

}  // namespace geometry

#endif  // GEOMETRY_TYPES_H
