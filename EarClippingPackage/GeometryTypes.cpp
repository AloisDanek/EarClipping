#include "GeometryTypes.h"

namespace geometry {

// ----------------------------------------------------------------------------
// Returns the previous index in a circular polygon vertex list.
// ----------------------------------------------------------------------------
size_t PreviousIndex(size_t i, size_t n)
{
  return (i + n - 1) % n;
}

// ----------------------------------------------------------------------------
// Returns the next index in a circular polygon vertex list.
// ----------------------------------------------------------------------------
size_t NextIndex(size_t i, size_t n)
{
  return (i + 1) % n;
}

}  // namespace geometry
