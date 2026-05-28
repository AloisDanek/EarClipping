#ifndef KD_TREE_H
#define KD_TREE_H

#include "Bounds.h"
#include "Point.h"

#include <memory>
#include <vector>

namespace geometry {

class KDTree {
 public:
  explicit KDTree(const std::vector<Point>& points);

  std::vector<size_t> Query(const Bounds& bounds) const;

 private:
  struct Node {
    size_t point_index = 0;
    int axis = 0;
    std::unique_ptr<Node> left;
    std::unique_ptr<Node> right;
  };

  std::unique_ptr<Node> Build(std::vector<size_t> indices, int depth) const;
  void Query(const Node* node, const Bounds& bounds, std::vector<size_t>& result) const;

  const std::vector<Point>& points_;
  std::unique_ptr<Node> root_;
};

}  // namespace geometry

#endif  // KD_TREE_H
