#pragma once

#include <algorithm>
#include <cmath>
#include <limits>

namespace arp_greedy::detail {

// Outward-rounded enclosures under IEEE binary64, round-to-nearest, no fast-math.
// Non-finite intermediate values deliberately force an exact fallback.
struct Interval {
  double lo = 0;
  double hi = 0;
  Interval() = default;
  explicit Interval(double x) : lo(x), hi(x) {}
  Interval(double low, double high) : lo(low), hi(high) {}
  static Interval whole() {
    const double inf = std::numeric_limits<double>::infinity();
    return {-inf, inf};
  }
  bool finite() const { return std::isfinite(lo) && std::isfinite(hi); }
  int certain_sign() const { return lo > 0 ? 1 : (hi < 0 ? -1 : 0); }
};

inline Interval enclose(double low, double high) {
  if (!std::isfinite(low) || !std::isfinite(high))
    return Interval::whole();
  const double inf = std::numeric_limits<double>::infinity();
  return {std::nextafter(low, -inf), std::nextafter(high, inf)};
}
inline Interval operator+(Interval a, Interval b) {
  if (!a.finite() || !b.finite())
    return Interval::whole();
  return enclose(a.lo + b.lo, a.hi + b.hi);
}
inline Interval operator-(Interval a, Interval b) {
  if (!a.finite() || !b.finite())
    return Interval::whole();
  return enclose(a.lo - b.hi, a.hi - b.lo);
}
inline Interval operator-(Interval a) { return {-a.hi, -a.lo}; }
inline Interval operator*(Interval a, Interval b) {
  if (!a.finite() || !b.finite())
    return Interval::whole();
  const double v[] = {a.lo * b.lo, a.lo * b.hi, a.hi * b.lo, a.hi * b.hi};
  for (double x : v)
    if (!std::isfinite(x))
      return Interval::whole();
  return enclose(*std::min_element(v, v + 4), *std::max_element(v, v + 4));
}
inline Interval operator/(Interval a, Interval b) {
  if (!a.finite() || !b.finite() || b.lo <= 0)
    return Interval::whole();
  const double v[] = {a.lo / b.lo, a.lo / b.hi, a.hi / b.lo, a.hi / b.hi};
  for (double x : v)
    if (!std::isfinite(x))
      return Interval::whole();
  return enclose(*std::min_element(v, v + 4), *std::max_element(v, v + 4));
}

} // namespace arp_greedy::detail
