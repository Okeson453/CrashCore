#pragma once
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif


/**
 * V1 full-scan feature calculators — complete port of calculators.ts.
 * Self-contained; no imports from outside pe/.
 */

#include "pe/types.hpp"

#include <algorithm>
#include <cmath>
#include <ctime>
#include <string>
#include <unordered_map>
#include <vector>

namespace pe {

inline double mean(const std::vector<double>& values) {
  if (values.empty()) return 0.0;
  double s = 0.0;
  for (double v : values) s += v;
  return s / static_cast<double>(values.size());
}

inline double median(std::vector<double> values) {
  if (values.empty()) return 0.0;
  std::sort(values.begin(), values.end());
  const size_t n = values.size();
  const size_t mid = n / 2;
  if (n % 2 == 0) return (values[mid - 1] + values[mid]) / 2.0;
  return values[mid];
}

inline double variance(const std::vector<double>& values) {
  if (values.size() < 2) return 0.0;
  const double m = mean(values);
  double acc = 0.0;
  for (double v : values) { const double d = v - m; acc += d * d; }
  return acc / static_cast<double>(values.size() - 1);
}

inline double stddev(const std::vector<double>& values) {
  return std::sqrt(variance(values));
}

inline double percentile(std::vector<double> values, double p) {
  if (values.empty()) return 0.0;
  std::sort(values.begin(), values.end());
  const double idx = (p / 100.0) * static_cast<double>(values.size() - 1);
  const size_t lo = static_cast<size_t>(std::floor(idx));
  const size_t hi = static_cast<size_t>(std::ceil(idx));
  if (lo == hi) return values[lo];
  return values[lo] + (values[hi] - values[lo]) * (idx - static_cast<double>(lo));
}

inline double hitRateRounds(const std::vector<HistoricalRound>& rounds, double threshold) {
  if (rounds.empty()) return 0.0;
  int h = 0;
  for (const auto& r : rounds) if (r.crashPoint >= threshold) ++h;
  return static_cast<double>(h) / static_cast<double>(rounds.size());
}

inline double hitRatePoints(const std::vector<double>& cps, double threshold) {
  if (cps.empty()) return 0.0;
  int h = 0;
  for (double c : cps) if (c >= threshold) ++h;
  return static_cast<double>(h) / static_cast<double>(cps.size());
}

inline int roundsSinceRounds(const std::vector<HistoricalRound>& rounds, double threshold) {
  for (int i = static_cast<int>(rounds.size()) - 1; i >= 0; --i) {
    if (rounds[static_cast<size_t>(i)].crashPoint >= threshold)
      return static_cast<int>(rounds.size()) - 1 - i;
  }
  return static_cast<int>(rounds.size());
}

template <typename Pred>
inline int consecutiveStreak(const std::vector<HistoricalRound>& rounds, Pred pred) {
  int streak = 0;
  for (int i = static_cast<int>(rounds.size()) - 1; i >= 0; --i) {
    if (pred(rounds[static_cast<size_t>(i)].crashPoint)) ++streak;
    else break;
  }
  return streak;
}

inline int hourOfDayIso(const std::string& iso) {
  if (iso.size() < 13) return -1;
  try { return std::stoi(iso.substr(11, 2)); } catch (...) { return -1; }
}

inline int dayOfWeekIso(const std::string& iso) {
  if (iso.size() < 10) return -1;
  std::tm tm{};
  if (strptime(iso.c_str(), "%Y-%m-%d", &tm) == nullptr) return -1;
  // Zeller-ish via timegm
  time_t t = timegm(&tm);
  if (t == static_cast<time_t>(-1)) return -1;
  std::tm out{};
  gmtime_r(&t, &out);
  return out.tm_wday;
}

inline double parseIsoMs(const std::string& iso) {
  if (iso.empty()) return NAN;
  std::tm tm{};
  const char* rest = strptime(iso.c_str(), "%Y-%m-%dT%H:%M:%S", &tm);
  if (!rest) rest = strptime(iso.c_str(), "%Y-%m-%d", &tm);
  if (!rest) return NAN;
  time_t t = timegm(&tm);
  if (t == static_cast<time_t>(-1)) return NAN;
  return static_cast<double>(t) * 1000.0;
}

/** Full V1 feature bag matching calculators.ts computeFeatures. */
inline std::unordered_map<std::string, double> computeFeatures(
    const std::vector<HistoricalRound>& priorRounds,
    const std::string& predictionTimestamp) {
  std::vector<double> cps;
  cps.reserve(priorRounds.size());
  for (const auto& r : priorRounds) cps.push_back(r.crashPoint);

  auto slice = [&](int n) -> std::vector<double> {
    if (cps.empty()) return {};
    const int start = std::max(0, static_cast<int>(cps.size()) - n);
    return {cps.begin() + start, cps.end()};
  };
  auto sliceRounds = [&](int n) -> std::vector<HistoricalRound> {
    if (priorRounds.empty()) return {};
    const int start = std::max(0, static_cast<int>(priorRounds.size()) - n);
    return {priorRounds.begin() + start, priorRounds.end()};
  };

  const auto last10 = slice(10);
  const auto last50 = slice(50);
  const auto last100 = slice(100);

  double quality = 0.0;
  if (!priorRounds.empty()) {
    double s = 0.0;
    for (const auto& r : priorRounds) {
      if (!r.dataQuality) { s += 0.3; continue; }
      switch (*r.dataQuality) {
        case DataQuality::High:   s += 1.0; break;
        case DataQuality::Medium: s += 0.6; break;
        case DataQuality::Low:    s += 0.3; break;
      }
    }
    quality = s / static_cast<double>(priorRounds.size());
  }

  double secondsSincePrev = 0.0;
  if (!priorRounds.empty()) {
    const auto& last = priorRounds.back();
    const std::string tLastStr = last.crashedAt.value_or(last.startedAt.value_or(last.createdAt));
    const double tLast = parseIsoMs(tLastStr);
    const double tNow = parseIsoMs(predictionTimestamp);
    if (std::isfinite(tLast) && std::isfinite(tNow))
      secondsSincePrev = std::max(0.0, std::min(600.0, (tNow - tLast) / 1000.0));
  }

  double roundsPerHour = 0.0;
  if (priorRounds.size() >= 5) {
    const size_t win = std::min(priorRounds.size(), size_t{50});
    const auto& first = priorRounds[priorRounds.size() - win];
    const auto& last = priorRounds.back();
    const double t0 = parseIsoMs(first.startedAt.value_or(first.createdAt));
    const double t1 = parseIsoMs(last.startedAt.value_or(last.createdAt));
    if (std::isfinite(t0) && std::isfinite(t1) && t1 > t0) {
      const double hours = (t1 - t0) / 3'600'000.0;
      if (hours > 1e-6) roundsPerHour = static_cast<double>(win) / hours;
    }
  }

  return {
    {"roll_mean_50", mean(last50)},
    {"roll_median_50", median(last50)},
    {"roll_min_50", last50.empty() ? 0.0 : *std::min_element(last50.begin(), last50.end())},
    {"roll_max_50", last50.empty() ? 0.0 : *std::max_element(last50.begin(), last50.end())},
    {"roll_std_50", stddev(last50)},
    {"roll_var_50", variance(last50)},
    {"roll_p25_50", percentile(last50, 25)},
    {"roll_p75_50", percentile(last50, 75)},
    {"roll_p90_50", percentile(last50, 90)},
    {"roll_mean_10", mean(last10)},
    {"roll_std_10", stddev(last10)},
    {"roll_mean_100", mean(last100)},
    {"roll_std_100", stddev(last100)},
    {"hit_1_30_50", hitRateRounds(sliceRounds(50), 1.3)},
    {"hit_2_00_50", hitRateRounds(sliceRounds(50), 2.0)},
    {"hit_5_00_50", hitRateRounds(sliceRounds(50), 5.0)},
    {"hit_10_00_50", hitRateRounds(sliceRounds(50), 10.0)},
    {"hit_1_30_100", hitRateRounds(sliceRounds(100), 1.3)},
    {"hit_2_00_100", hitRateRounds(sliceRounds(100), 2.0)},
    {"since_1_30", static_cast<double>(roundsSinceRounds(priorRounds, 1.3))},
    {"since_2_00", static_cast<double>(roundsSinceRounds(priorRounds, 2.0))},
    {"since_5_00", static_cast<double>(roundsSinceRounds(priorRounds, 5.0))},
    {"since_10_00", static_cast<double>(roundsSinceRounds(priorRounds, 10.0))},
    {"consec_below_1_30", static_cast<double>(consecutiveStreak(priorRounds, [](double cp){ return cp < 1.3; }))},
    {"consec_below_2_00", static_cast<double>(consecutiveStreak(priorRounds, [](double cp){ return cp < 2.0; }))},
    {"consec_above_2_00", static_cast<double>(consecutiveStreak(priorRounds, [](double cp){ return cp >= 2.0; }))},
    {"consec_above_5_00", static_cast<double>(consecutiveStreak(priorRounds, [](double cp){ return cp >= 5.0; }))},
    {"sample_size", static_cast<double>(cps.size())},
    {"quality_score", quality},
    {"hour_utc", static_cast<double>(hourOfDayIso(predictionTimestamp))},
    {"dow_utc", static_cast<double>(dayOfWeekIso(predictionTimestamp))},
    {"seconds_since_prev", secondsSincePrev},
    {"rounds_per_hour_est", roundsPerHour},
  };
}

inline std::unordered_map<std::string, double> computeV1Features(const std::vector<double>& points) {
  std::unordered_map<std::string, double> out;
  const int n = static_cast<int>(points.size());
  out["sample_size"] = static_cast<double>(n);
  if (n == 0) {
    out["mean_crash"] = 0.0; out["std_crash"] = 0.0;
    out["hit_13"] = 0.0; out["hit_20"] = 0.0; out["hit_50"] = 0.0;
    out["quality_score"] = 0.0;
    return out;
  }
  out["mean_crash"] = mean(points);
  out["std_crash"] = stddev(points);
  out["hit_13"] = hitRatePoints(points, 1.3);
  out["hit_20"] = hitRatePoints(points, 2.0);
  out["hit_50"] = hitRatePoints(points, 5.0);
  out["quality_score"] = std::min(1.0, static_cast<double>(n) / 100.0);
  return out;
}

} // namespace pe
