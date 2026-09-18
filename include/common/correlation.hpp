#pragma once
#include "timing/timestamp.hpp"
#include <atomic>
#include <string>
#include <sstream>
#include <random>

namespace crashcore {

inline std::string makeCorrelationId(const std::string& prefix = "cc") {
  static std::atomic<std::uint64_t> seq{0};
  std::ostringstream oss;
  oss << prefix << "-" << nowMs() << "-" << seq.fetch_add(1);
  return oss.str();
}

inline std::string makePredictionId(const std::string& roundId) {
  return "pred-" + roundId + "-" + makeCorrelationId("p");
}

} // namespace crashcore
