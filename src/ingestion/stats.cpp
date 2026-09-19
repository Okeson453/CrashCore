#include "ingestion/stats.hpp"

namespace crashcore {

// Explicit symbols so the translation unit is not empty under LTO.
const char* kCrashStatsModule = "crashcore.ingestion.stats";

double crashStatsAverageOr(const CrashStatsSnapshot& s, double fallback) {
  return s.average.value_or(fallback);
}

std::uint64_t crashStatsBucketCount(const std::vector<RangeBucket>& buckets, const std::string& key) {
  for (const auto& b : buckets) {
    if (b.key == key) return b.count;
  }
  return 0;
}

} // namespace crashcore
