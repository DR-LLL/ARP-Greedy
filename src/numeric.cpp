#include "numeric.hpp"
#include <cfenv>
#include <cmath>
#include <cstring>
#include <limits>
#include <stdexcept>

namespace arp_greedy::detail {

static_assert(std::numeric_limits<double>::is_iec559 && std::numeric_limits<double>::digits == 53 &&
                  sizeof(double) == 8,
              "ARP-Greedy requires IEEE 754 binary64 doubles");

Rational exact_double(double value) {
  if (value == 0)
    return 0;
  int exponent = 0;
  const double mantissa = std::frexp(value, &exponent);
  const auto significand = static_cast<std::int64_t>(std::ldexp(mantissa, 53));
  Rational result(significand);
  exponent -= 53;
  if (exponent >= 0)
    result *= Integer(1) << exponent;
  else
    result /= Integer(1) << -exponent;
  return result;
}

void validate(const std::vector<Job> &jobs) {
  if (std::fegetround() != FE_TONEAREST)
    throw std::invalid_argument("ARP-Greedy requires round-to-nearest arithmetic");
  // Volatile forces an actual operation. Inspect bits so DAZ cannot make the
  // comparison itself treat a subnormal expected value as zero.
  volatile double tiny = std::numeric_limits<double>::denorm_min();
  const double twice_tiny = tiny + tiny;
  std::uint64_t bits = 0;
  std::memcpy(&bits, &twice_tiny, sizeof(bits));
  if (bits != 2)
    throw std::invalid_argument("ARP-Greedy requires gradual underflow (disable FTZ/DAZ)");
  for (const auto &job : jobs) {
    if (!std::isfinite(job.processing_time) || job.processing_time <= 0 ||
        !std::isfinite(job.weight) || job.weight < 0)
      throw std::invalid_argument("Expected finite processing_time > 0 and weight >= 0");
  }
}

Point make_point(const Job &job, std::size_t index) {
  const Rational p = exact_double(job.processing_time);
  const Rational w = exact_double(job.weight);
  const Interval ip(job.processing_time), iw(job.weight);
  return {p / w, -p * p / w, ip / iw, -(ip * ip / iw), index};
}

int Predicates::x_compare(const Point &a, const Point &b) {
  return sign(a.ix - b.ix, [&]() -> Rational { return a.x - b.x; });
}
int Predicates::y_compare(const Point &a, const Point &b) {
  return sign(a.iy - b.iy, [&]() -> Rational { return a.y - b.y; });
}
int Predicates::orientation(const Point &a, const Point &b, const Point &c) {
  return direction_cross(a, b, a, c);
}
int Predicates::direction_cross(const Point &a, const Point &b, const Point &c, const Point &d) {
  return sign((b.ix - a.ix) * (d.iy - c.iy) - (b.iy - a.iy) * (d.ix - c.ix),
              [&]() -> Rational { return (b.x - a.x) * (d.y - c.y) - (b.y - a.y) * (d.x - c.x); });
}
int Predicates::value_compare(const Point &a, const Point &b, const Time &time) {
  return sign((a.ix - b.ix) * time.enclosure - (a.iy - b.iy),
              [&]() -> Rational { return (a.x - b.x) * time.exact - (a.y - b.y); });
}

void evaluate(const std::vector<Job> &jobs, Result &result) {
  Rational time = 0;
  double total = 0, correction = 0;
  for (std::size_t i : result.order) {
    time += exact_double(jobs[i].processing_time);
    const Rational term = exact_double(jobs[i].weight) / time;
    const double rounded = term.convert_to<double>();
    if (!std::isfinite(rounded)) {
      result.objective_overflow = true;
      break;
    }
    const double adjusted = rounded - correction;
    const double next = total + adjusted;
    if (!std::isfinite(next)) {
      result.objective_overflow = true;
      break;
    }
    correction = (next - total) - adjusted;
    total = next;
  }
  result.objective = result.objective_overflow ? std::numeric_limits<double>::infinity() : total;
}

} // namespace arp_greedy::detail
