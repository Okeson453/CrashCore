#pragma once
/**
 * PORT_BEHAVIOR from TestingEngine src/lib/crash/stats.ts + types.ts RANGE_DEFS
 */
#include "common/types.hpp"
#include <algorithm>
#include <cmath>
#include <mutex>
#include <string>
#include <vector>
#include <optional>

namespace crashcore {

struct RangeBucket {
  std::string key;
  std::string label;
  double min = 0;
  std::optional<double> max;
  std::uint64_t count = 0;
  double pct = 0;
};

struct StreakSnapshot {
  enum class Kind { None, Low, High } currentKind = Kind::None;
  int currentCount = 0;
  int maxLow = 0;
  int maxHigh = 0;
  double threshold = HIGH_THRESHOLD;
};

struct CrashStatsSnapshot {
  std::uint64_t count = 0;
  std::optional<double> average;
  std::optional<double> median;
  std::optional<double> highest;
  std::optional<double> lowest;
};

inline std::vector<RangeBucket> defaultRangeDefs() {
  return {
    {"1-1.2", "1.00–1.20×", 1.0, 1.2, 0, 0},
    {"1.2-1.5", "1.20–1.50×", 1.2, 1.5, 0, 0},
    {"1.5-2", "1.50–2.00×", 1.5, 2.0, 0, 0},
    {"2-3", "2.00–3.00×", 2.0, 3.0, 0, 0},
    {"3-5", "3.00–5.00×", 3.0, 5.0, 0, 0},
    {"5-10", "5.00–10.00×", 5.0, 10.0, 0, 0},
    {"10-50", "10.00–50.00×", 10.0, 50.0, 0, 0},
    {"50+", "50.00×+", 50.0, std::nullopt, 0, 0},
  };
}

class CrashStatsAccumulator {
public:
  CrashStatsAccumulator() : ranges_(defaultRangeDefs()) {}

  void addRound(double multiplier) {
    std::lock_guard lk(mu_);
    values_.push_back(multiplier);
    ++count_;
    sum_ += multiplier;
    if (!highest_ || multiplier > *highest_) highest_ = multiplier;
    if (!lowest_ || multiplier < *lowest_) lowest_ = multiplier;
    for (auto& b : ranges_) {
      if (multiplier >= b.min && (!b.max || multiplier < *b.max)) {
        ++b.count;
        break;
      }
    }
    // streaks
    if (multiplier < HIGH_THRESHOLD) {
      if (streak_.currentKind == StreakSnapshot::Kind::Low) ++streak_.currentCount;
      else { streak_.currentKind = StreakSnapshot::Kind::Low; streak_.currentCount = 1; }
      streak_.maxLow = std::max(streak_.maxLow, streak_.currentCount);
    } else {
      if (streak_.currentKind == StreakSnapshot::Kind::High) ++streak_.currentCount;
      else { streak_.currentKind = StreakSnapshot::Kind::High; streak_.currentCount = 1; }
      streak_.maxHigh = std::max(streak_.maxHigh, streak_.currentCount);
    }
  }

  CrashStatsSnapshot snapshot() const {
    std::lock_guard lk(mu_);
    CrashStatsSnapshot s;
    s.count = count_;
    if (count_ > 0) {
      s.average = sum_ / static_cast<double>(count_);
      auto sorted = values_;
      std::sort(sorted.begin(), sorted.end());
      s.median = sorted[sorted.size() / 2];
      s.highest = highest_;
      s.lowest = lowest_;
    }
    return s;
  }

  std::vector<RangeBucket> ranges() const {
    std::lock_guard lk(mu_);
    auto out = ranges_;
    for (auto& b : out) {
      b.pct = count_ ? (100.0 * static_cast<double>(b.count) / static_cast<double>(count_)) : 0.0;
    }
    return out;
  }

  StreakSnapshot streaks() const {
    std::lock_guard lk(mu_);
    return streak_;
  }

  std::uint64_t count() const {
    std::lock_guard lk(mu_);
    return count_;
  }

private:
  mutable std::mutex mu_;
  std::vector<double> values_;
  std::vector<RangeBucket> ranges_;
  StreakSnapshot streak_;
  std::uint64_t count_ = 0;
  double sum_ = 0;
  std::optional<double> highest_, lowest_;
};

} // namespace crashcore
