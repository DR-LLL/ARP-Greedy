#include "arp_greedy/greedy.hpp"
#include "decremental_hull.hpp"
#include "numeric.hpp"
#include <algorithm>

namespace arp_greedy {

Result schedule(const std::vector<Job> &jobs, Options options) {
  detail::validate(jobs);
  Result result;
  result.order.reserve(jobs.size());
  detail::Predicates predicates(options.arithmetic, result.statistics);
  std::vector<detail::Point> points;
  points.reserve(jobs.size());
  std::vector<std::size_t> zeros;
  for (std::size_t i = 0; i < jobs.size(); ++i) {
    if (jobs[i].weight == 0)
      zeros.push_back(i);
    else
      points.push_back(detail::make_point(jobs[i], i));
  }
  std::sort(points.begin(), points.end(), [&](const auto &a, const auto &b) {
    int comparison = predicates.x_compare(a, b);
    if (comparison == 0)
      comparison = predicates.y_compare(a, b);
    return comparison != 0 ? comparison < 0 : a.input_index < b.input_index;
  });

  // Identical points have identical p and w. Keep all jobs but one hull vertex.
  std::vector<std::vector<std::size_t>> groups;
  std::vector<detail::Point> unique;
  unique.reserve(points.size());
  for (auto &point : points) {
    if (unique.empty() || predicates.x_compare(unique.back(), point) != 0 ||
        predicates.y_compare(unique.back(), point) != 0) {
      groups.push_back({point.input_index});
      unique.push_back(std::move(point));
    } else {
      groups.back().push_back(point.input_index);
    }
  }
  points.clear();
  points.shrink_to_fit();
  std::vector<std::size_t> cursor(groups.size(), 0);
  detail::DecrementalHull hull(unique, predicates, result.statistics);
  detail::Time time;
  while (!hull.empty()) {
    const auto vertex = hull.minimum(time);
    const auto index = groups[vertex][cursor[vertex]++];
    result.order.push_back(index);
    if (cursor[vertex] == groups[vertex].size())
      hull.erase(vertex);
    time.add(jobs[index].processing_time);
  }
  result.order.insert(result.order.end(), zeros.begin(), zeros.end());
  detail::evaluate(jobs, result);
  return result;
}

std::vector<std::size_t> departure_order(const Result &result) {
  return {result.order.rbegin(), result.order.rend()};
}

} // namespace arp_greedy
