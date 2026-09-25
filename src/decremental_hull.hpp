#pragma once

#include "numeric.hpp"
#include <limits>
#include <utility>

namespace arp_greedy::detail {

// Split-chain decremental upper hull (Hershberger--Suri).
// Chain maintenance adapted from ei1333/library, released under the Unlicense.
// See THIRD_PARTY_NOTICES.md. Indices replace pointers; bridges also support
// logarithmic extreme-point queries without materializing the hull.
class DecrementalHull {
public:
  DecrementalHull(const std::vector<Point> &points, Predicates &predicates, Statistics &statistics);
  std::size_t minimum(const Time &time);
  void erase(std::size_t index);
  bool empty() const { return points_.empty() || nodes_[1].live == 0; }

private:
  static constexpr std::size_t none = std::numeric_limits<std::size_t>::max();
  struct Link {
    std::size_t prev = none, next = none;
  };
  struct Node {
    std::size_t head = none, tail = none;
    std::size_t bridge_left = none, bridge_right = none;
    std::size_t live = 0;
  };
  const std::vector<Point> &points_;
  Predicates &predicates_;
  Statistics &statistics_;
  std::vector<Link> links_;
  std::vector<Node> nodes_;
  std::vector<bool> alive_;

  std::pair<std::size_t, std::size_t> bridge_forward(std::size_t l, std::size_t r);
  std::pair<std::size_t, std::size_t> bridge_backward(std::size_t l, std::size_t r);
  void join(std::size_t node, std::size_t l, std::size_t r, bool backward);
  void split(std::size_t node);
  void take_chain(std::size_t node, std::size_t child);
  void build(std::size_t node, std::size_t begin, std::size_t end);
  void erase_at(std::size_t node, std::size_t begin, std::size_t end, std::size_t index);
};

} // namespace arp_greedy::detail
