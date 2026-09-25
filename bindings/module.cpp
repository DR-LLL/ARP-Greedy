#include "arp_greedy/greedy.hpp"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <stdexcept>
#include <string>
#include <utility>

namespace py = pybind11;

PYBIND11_MODULE(_native, module) {
  module.doc() = "Exact-choice C++ core for arp_greedy; use the public Python API.";
  module.attr("__version__") = ARP_GREEDY_VERSION;
  module.def(
      "solve",
      [](const std::vector<std::pair<double, double>> &pairs, const std::string &arithmetic) {
        arp_greedy::Options options;
        if (arithmetic == "exact")
          options.arithmetic = arp_greedy::Arithmetic::exact;
        else if (arithmetic != "filtered")
          throw std::invalid_argument("arithmetic must be 'filtered' or 'exact'");
        std::vector<arp_greedy::Job> jobs;
        jobs.reserve(pairs.size());
        for (const auto &pair : pairs)
          jobs.push_back({pair.first, pair.second});
        arp_greedy::Result result;
        {
          py::gil_scoped_release release;
          result = arp_greedy::schedule(jobs, options);
        }
        const auto &stats = result.statistics;
        py::dict counters;
        counters["predicates"] = stats.predicates;
        counters["exact_fallbacks"] = stats.exact_fallbacks;
        counters["bridge_steps"] = stats.bridge_steps;
        counters["query_nodes"] = stats.query_nodes;
        counters["repaired_nodes"] = stats.repaired_nodes;
        return py::make_tuple(result.order, result.objective, result.objective_overflow, counters);
      },
      py::arg("pairs"), py::arg("arithmetic") = "filtered");
}
