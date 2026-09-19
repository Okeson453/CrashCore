#include "timing/latency_tracker.hpp"

namespace crashcore {

const char* kLatencyTrackerModule = "crashcore.timing.latency_tracker";

std::uint64_t latencyTrackerCount(const LatencyTracker& t, LatencyTracker::Stage stage) {
  return t.stats(stage).count;
}

double latencyTrackerAvgUs(const LatencyTracker& t, LatencyTracker::Stage stage) {
  return t.stats(stage).avgUs;
}

} // namespace crashcore
