#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

namespace arp_greedy {

struct Job {
  double processing_time;
  double weight;
};

enum class Arithmetic { filtered, exact };

struct Options {
  Arithmetic arithmetic = Arithmetic::filtered;
};

struct Statistics {
  std::uint64_t predicates = 0;
  std::uint64_t exact_fallbacks = 0;
  std::uint64_t bridge_steps = 0;
  std::uint64_t query_nodes = 0;
  std::uint64_t repaired_nodes = 0;
};

struct Result {
  // Zero-based input indices, in single-machine execution order.
  std::vector<std::size_t> order;
  // Rounded diagnostic, never used for scheduling decisions.
  double objective = 0;
  bool objective_overflow = false;
  Statistics statistics;
};

// Maximize sum_i w_i/C_i with the sequential maximum-phi greedy rule.
// Finite binary64 inputs: processing_time > 0, weight >= 0.
// Ties: smaller exact p/w first, then smaller input index. Zero weights last.
// Exact choices for the represented inputs; no epsilon-based comparisons.
// Throws std::invalid_argument for invalid input or unsupported rounding mode.
Result schedule(const std::vector<Job> &jobs, Options options = {});

// The aircraft departure order is the reverse of the scheduling order.
std::vector<std::size_t> departure_order(const Result &result);

} // namespace arp_greedy
