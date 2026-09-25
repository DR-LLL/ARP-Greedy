#include "decremental_hull.hpp"
#include <stdexcept>

namespace arp_greedy::detail {

DecrementalHull::DecrementalHull(const std::vector<Point> &points, Predicates &predicates,
                                 Statistics &statistics)
    : points_(points), predicates_(predicates), statistics_(statistics), links_(points.size()),
      alive_(points.size(), true) {
  if (points.size() > nodes_.max_size() / 4)
    throw std::length_error("Too many points for the hull");
  nodes_.resize(points.empty() ? 2 : 4 * points.size());
  if (!points.empty())
    build(1, 0, points.size());
}

std::pair<std::size_t, std::size_t> DecrementalHull::bridge_forward(std::size_t l, std::size_t r) {
  while (links_[l].next != none || links_[r].next != none) {
    const auto ln = links_[l].next, rn = links_[r].next;
    if (rn == none || (ln != none && predicates_.direction_cross(points_[l], points_[ln],
                                                                 points_[r], points_[rn]) <= 0)) {
      if (predicates_.orientation(points_[l], points_[ln], points_[r]) > 0)
        break;
      l = ln;
    } else {
      if (predicates_.orientation(points_[l], points_[r], points_[rn]) <= 0)
        break;
      r = rn;
    }
    ++statistics_.bridge_steps;
  }
  return {l, r};
}

std::pair<std::size_t, std::size_t> DecrementalHull::bridge_backward(std::size_t l, std::size_t r) {
  while (links_[r].prev != none || links_[l].prev != none) {
    const auto lp = links_[l].prev, rp = links_[r].prev;
    if (lp == none || (rp != none && predicates_.direction_cross(points_[r], points_[rp],
                                                                 points_[l], points_[lp]) >= 0)) {
      if (predicates_.orientation(points_[r], points_[rp], points_[l]) < 0)
        break;
      r = rp;
    } else {
      if (predicates_.orientation(points_[r], points_[l], points_[lp]) >= 0)
        break;
      l = lp;
    }
    ++statistics_.bridge_steps;
  }
  return {l, r};
}

void DecrementalHull::join(std::size_t u, std::size_t l, std::size_t r, bool backward) {
  const auto bridge = backward ? bridge_backward(l, r) : bridge_forward(l, r);
  l = bridge.first;
  r = bridge.second;
  auto &node = nodes_[u];
  auto &left = nodes_[2 * u];
  auto &right = nodes_[2 * u + 1];
  node.bridge_left = l;
  node.bridge_right = r;
  node.head = left.head;
  node.tail = right.tail;
  node.live = left.live + right.live;

  // Promote the visible prefix/suffix; children retain only hidden chain pieces.
  left.head = links_[l].next;
  right.tail = links_[r].prev;
  if (left.head != none)
    links_[left.head].prev = none;
  else
    left.tail = none;
  if (right.tail != none)
    links_[right.tail].next = none;
  else
    right.head = none;
  links_[l].next = r;
  links_[r].prev = l;
}

void DecrementalHull::split(std::size_t u) {
  auto &node = nodes_[u];
  auto &left = nodes_[2 * u];
  auto &right = nodes_[2 * u + 1];
  const auto l = node.bridge_left, r = node.bridge_right;
  links_[l].next = left.head;
  if (left.head != none)
    links_[left.head].prev = l;
  else
    left.tail = l;
  left.head = node.head;
  links_[r].prev = right.tail;
  if (right.tail != none)
    links_[right.tail].next = r;
  else
    right.head = r;
  right.tail = node.tail;
}

void DecrementalHull::take_chain(std::size_t u, std::size_t child) {
  nodes_[u].head = nodes_[child].head;
  nodes_[u].tail = nodes_[child].tail;
  nodes_[u].live = nodes_[child].live;
  nodes_[u].bridge_left = nodes_[u].bridge_right = none;
  nodes_[child].head = nodes_[child].tail = none;
}

void DecrementalHull::build(std::size_t u, std::size_t begin, std::size_t end) {
  if (end - begin == 1) {
    nodes_[u].head = nodes_[u].tail = begin;
    nodes_[u].live = 1;
    return;
  }
  const auto mid = begin + (end - begin) / 2;
  build(2 * u, begin, mid);
  build(2 * u + 1, mid, end);
  join(u, nodes_[2 * u].head, nodes_[2 * u + 1].head, false);
}

void DecrementalHull::erase_at(std::size_t u, std::size_t begin, std::size_t end,
                               std::size_t index) {
  ++statistics_.repaired_nodes;
  if (end - begin == 1) {
    nodes_[u] = Node{};
    links_[index] = Link{};
    return;
  }
  const auto mid = begin + (end - begin) / 2;
  const bool in_left = index < mid;
  const auto child = in_left ? 2 * u : 2 * u + 1;
  if (nodes_[u].bridge_left == none) {
    nodes_[child].head = nodes_[u].head;
    nodes_[child].tail = nodes_[u].tail;
    erase_at(child, in_left ? begin : mid, in_left ? mid : end, index);
    take_chain(u, child);
    return;
  }

  auto l = nodes_[u].bridge_left, r = nodes_[u].bridge_right;
  split(u);
  if (in_left && l == index)
    l = links_[l].next;
  if (!in_left && r == index)
    r = links_[r].prev;
  erase_at(child, in_left ? begin : mid, in_left ? mid : end, index);

  if (nodes_[child].live == 0) {
    take_chain(u, in_left ? 2 * u + 1 : 2 * u);
    return;
  }
  if (l == none)
    l = nodes_[2 * u].tail;
  if (r == none)
    r = nodes_[2 * u + 1].head;
  join(u, l, r, in_left);
}

void DecrementalHull::erase(std::size_t index) {
  if (index >= alive_.size() || !alive_[index])
    throw std::logic_error("Invalid or repeated hull deletion");
  erase_at(1, 0, points_.size(), index);
  alive_[index] = false;
}

std::size_t DecrementalHull::minimum(const Time &time) {
  if (empty())
    throw std::logic_error("Query on an empty hull");
  std::size_t u = 1, begin = 0, end = points_.size();
  while (end - begin > 1) {
    ++statistics_.query_nodes;
    const auto &node = nodes_[u];
    bool left;
    if (node.bridge_left == none) {
      left = nodes_[2 * u].live != 0;
    } else {
      // The upper hull is concave. The sign on its bridge chooses the half
      // containing a maximum of y-S*x. Equality goes to the smaller slope.
      left = predicates_.value_compare(points_[node.bridge_left], points_[node.bridge_right],
                                       time) <= 0;
    }
    const auto mid = begin + (end - begin) / 2;
    if (left) {
      u *= 2;
      end = mid;
    } else {
      u = 2 * u + 1;
      begin = mid;
    }
  }
  return begin;
}

} // namespace arp_greedy::detail
