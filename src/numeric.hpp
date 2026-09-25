#pragma once

#include "arp_greedy/greedy.hpp"
#include "interval.hpp"
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/version.hpp>
#include <vector>

namespace arp_greedy::detail {

static_assert(BOOST_VERSION >= 107400, "ARP-Greedy requires Boost >= 1.74");

using Rational = boost::multiprecision::cpp_rational;
using Integer = boost::multiprecision::cpp_int;
Rational exact_double(double value);
void validate(const std::vector<Job> &jobs);

struct Point {
  Rational x, y;
  Interval ix, iy;
  std::size_t input_index;
};

struct Time {
  Rational exact = 0;
  Interval enclosure;
  void add(double p) {
    exact += exact_double(p);
    enclosure = enclosure + Interval(p);
  }
};

class Predicates {
public:
  Predicates(Arithmetic arithmetic, Statistics &statistics)
      : arithmetic_(arithmetic), statistics_(statistics) {}
  int x_compare(const Point &a, const Point &b);
  int y_compare(const Point &a, const Point &b);
  int orientation(const Point &a, const Point &b, const Point &c);
  int direction_cross(const Point &a, const Point &b, const Point &c, const Point &d);
  // Sign of reciprocal-priority(a) - reciprocal-priority(b).
  int value_compare(const Point &a, const Point &b, const Time &time);

private:
  template <class Exact> int sign(Interval enclosure, Exact exact) {
    ++statistics_.predicates;
    if (arithmetic_ == Arithmetic::filtered) {
      const int answer = enclosure.certain_sign();
      if (answer != 0)
        return answer;
    }
    ++statistics_.exact_fallbacks;
    const Rational value = exact();
    return value > 0 ? 1 : (value < 0 ? -1 : 0);
  }
  Arithmetic arithmetic_;
  Statistics &statistics_;
};

Point make_point(const Job &job, std::size_t index);
void evaluate(const std::vector<Job> &jobs, Result &result);

} // namespace arp_greedy::detail
