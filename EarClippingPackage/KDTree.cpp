#include "KDTree.h"

#include "GeometryTypes.h"

#include <algorithm>

namespace geometry {

// ----------------------------------------------------------------------------
// Builds a KD-tree over stable point indices.
// ----------------------------------------------------------------------------
KDTree::KDTree(const std::vector<Point>& points) : points_(points)
{
  std::vector<size_t> indices;
  indices.reserve(points_.size());
  for (size_t i = 0; i < points_.size(); ++i) {
    indices.push_back(i);
  }
  root_ = Build(std::move(indices), 0);
}

// ----------------------------------------------------------------------------
// Returns all vertex indices whose points are inside the query bounds.
// ----------------------------------------------------------------------------
std::vector<size_t> KDTree::Query(const Bounds& bounds) const
{
  std::vector<size_t> result;
  Query(root_.get(), bounds, result);
  return result;
}

// ----------------------------------------------------------------------------
// Recursively builds a balanced KD-tree by splitting on alternating axes.
// ----------------------------------------------------------------------------
std::unique_ptr<KDTree::Node> KDTree::Build(std::vector<size_t> indices,
                                            int depth) const
{
  if (indices.empty()) {
    return nullptr;
  }

  const int axis = depth % 2;
  const size_t median = indices.size() / 2;
  std::nth_element(indices.begin(), indices.begin() + median, indices.end(),
                   [this, axis](size_t left, size_t right) {
                     const Point& a = points_[left];
                     const Point& b = points_[right];
                     return axis == 0 ? a.x < b.x : a.y < b.y;
                   });

  auto node = std::make_unique<Node>();
  node->point_index = indices[median];
  node->axis = axis;

  std::vector<size_t> left_indices(indices.begin(), indices.begin() + median);
  std::vector<size_t> right_indices(indices.begin() + median + 1, indices.end());
  node->left = Build(std::move(left_indices), depth + 1);
  node->right = Build(std::move(right_indices), depth + 1);
  return node;
}

// ----------------------------------------------------------------------------
// Recursively collects points inside the bounds, pruning by the split axis.
// ----------------------------------------------------------------------------
void KDTree::Query(const Node* node,
                   const Bounds& bounds,
                   std::vector<size_t>& result) const
{
  if (node == nullptr) {
    return;
  }

  const Point& point = points_[node->point_index];
  if (bounds.Contains(point)) {
    result.push_back(node->point_index);
  }

  if (node->axis == 0) {
    if (bounds.MinX() <= point.x + kEps) {
      Query(node->left.get(), bounds, result);
    }
    if (bounds.MaxX() >= point.x - kEps) {
      Query(node->right.get(), bounds, result);
    }
  } else {
    if (bounds.MinY() <= point.y + kEps) {
      Query(node->left.get(), bounds, result);
    }
    if (bounds.MaxY() >= point.y - kEps) {
      Query(node->right.get(), bounds, result);
    }
  }
}

}  // namespace geometry
