#ifndef POLYGON_H
#define POLYGON_H

#include "GeometryTypes.h"
#include "Triangle.h"

#include <initializer_list>
#include <memory>
#include <vector>

namespace geometry {

class Bounds;
class KDTree;

class Polygon {
 public:
  Polygon();
  Polygon(std::initializer_list<Point> vertices);
  explicit Polygon(std::vector<Point> vertices);
  Polygon(const Polygon& other);
  Polygon& operator=(const Polygon& other);
  ~Polygon();

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
  Bounds GetBounds() const;
  const Point& PreviousVertex(size_t i) const;
  const Point& NextVertex(size_t i) const;
  void InitializeClipping();
  size_t RemainingVertices() const;
  bool IsClipped(size_t i) const;
  Polygon ActivePolygon() const;
  Triangle EarCandidateAt(size_t i) const;
  bool IsEar(size_t i) const;
  void ClipVertex(size_t i);
  Triangle FinalTriangle() const;

 private:
  void ResetClippingState();
  void RequireClippingInitialized() const;

  std::vector<Point> vertices_;
  std::vector<char> removed_;
  std::vector<size_t> previous_;
  std::vector<size_t> next_;
  std::unique_ptr<KDTree> tree_;
  size_t remaining_vertices_ = 0;
  bool clipping_initialized_ = false;
};

}  // namespace geometry

#endif  // POLYGON_H
