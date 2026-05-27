#ifndef POLYGON_H
#define POLYGON_H

#include "GeometryTypes.h"
#include "Triangle.h"

#include <initializer_list>
#include <vector>

namespace geometry {

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

}  // namespace geometry

#endif  // POLYGON_H
