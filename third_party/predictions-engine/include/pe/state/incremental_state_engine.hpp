#pragma once

/**
 * Phase 1 — Incremental State Engine
 * O(1) sufficient statistics for the prediction critical path.
 * Ported from incremental-state-engine.ts
 */

#include <array>
#include <cmath>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace pe {

inline constexpr std::array<double, 3> PREDICTION_TARGETS = {1.3, 2.0, 5.0};
using PredictionTarget = double;

struct HitCounters {
  int t13 = 0;
  int t20 = 0;
  int t50 = 0;
  int t100 = 0;
};

struct RunState {
  int below13 = 0;
  int above13 = 0;
  int below15 = 0;
  int below20 = 0;
  int above20 = 0;
  int maxBelow13 = 0;
  int maxAbove13 = 0;
  int maxBelow20 = 0;
};

struct SinceCounters {
  int t13 = 0;
  int t20 = 0;
  int t50 = 0;
  int t100 = 0;
};

enum class EngineLifecycleState {
  COLD,       // no observations yet (count === 0)
  WARMING,    // partial state, V1 fallback still in use (1..19)
  WARM,       // V2 incremental path active, building confidence (20..99)
  PRODUCTION, // fully warmed, all features real (≥100)
  DEGRADED    // stale state (no update in >5 min while count > 0)
};

inline const char* toString(EngineLifecycleState s) {
  switch (s) {
    case EngineLifecycleState::COLD:       return "COLD";
    case EngineLifecycleState::WARMING:    return "WARMING";
    case EngineLifecycleState::WARM:       return "WARM";
    case EngineLifecycleState::PRODUCTION: return "PRODUCTION";
    case EngineLifecycleState::DEGRADED:   return "DEGRADED";
  }
  return "UNKNOWN";
}

inline constexpr int64_t DEGRADED_TIMEOUT_MS = 5 * 60 * 1000; // 5 minutes

struct MarkovState {
  /** 2x2 on (prev>=1.3, curr>=1.3): [fromBelow][toAbove] counts */
  std::array<std::array<int, 2>, 2> trans{{{{0, 0}}, {{0, 0}}}};
  std::optional<bool> lastAbove13;
};

struct WelfordState {
  int n = 0;
  double mean = 0.0;
  double m2 = 0.0;
};

struct IncrementalEngineSnapshot {
  int count = 0;
  std::optional<double> lastCrash;
  WelfordState welford;
  double ewma = 1.3;
  double ewmaHit13 = 0.65;
  HitCounters hits;
  RunState runs;
  SinceCounters since;
  MarkovState markov;
  std::vector<double> lagRing; // oldest→newest, size = lagRingSize
  int lagRingSize = 0;
  double shortSum = 0.0;
  double shortSumSq = 0.0;
  int shortHits13 = 0;
  int shortCount = 0;
  int64_t updatedAt = 0;
  std::string featureVersion = "inc-state-v1";
  double lastGapS = 0.0;
  int gapCount = 0;
};

inline constexpr int LAG_CAP = 512;
inline constexpr int SHORT_CAP = 30;
inline constexpr double EWMA_ALPHA = 0.05;

class IncrementalStateEngine {
public:
  const std::string featureVersion = "inc-state-v1";

  void seed(const std::vector<double>& crashPoints);
  void recordBeganAt(int64_t beganAtMs);
  struct GapState {
    double lastGapS = 0.0;
    int gapCount = 0;
    std::optional<int64_t> lastBeganAtMs;
  };
  GapState getGapState() const;
  void reset();
  IncrementalEngineSnapshot update(double crashPoint);
  IncrementalEngineSnapshot snapshot() const;

  std::vector<double> getLagArray() const;
  double variance() const;
  double std() const;
  double hitRate(double target) const;
  int roundsSince(double target) const;
  double windowHitRate(int window, double target = 1.3) const;
  struct HitRateWindows13 {
    double w20 = 0.0;
    double w50 = 0.0;
    double w100 = 0.0;
    double w200 = 0.0;
  };
  HitRateWindows13 hitRateWindows13() const;
  double shortHitRate13() const;
  double shortMean() const;
  double shortVariance() const;
  double markovPNextAbove13() const;
  bool isWarm(int minCount = 50) const;
  EngineLifecycleState getLifecycleState() const;
  std::vector<double> getRecentPoints(int max = LAG_CAP) const;

  // Direct accessors used by feature families (avoid full snapshot copy on hot path)
  int count() const { return count_; }
  const HitCounters& hits() const { return hits_; }
  const RunState& runs() const { return runs_; }
  const SinceCounters& since() const { return since_; }
  const MarkovState& markov() const { return markov_; }
  double ewma() const { return ewma_; }
  double ewmaHit13() const { return ewmaHit13_; }
  const WelfordState& welford() const { return welford_; }
  double lastGapS() const { return lastGapS_; }
  int gapCount() const { return gapCount_; }
  int lagLen() const { return lagLen_; }
  int shortLen() const { return shortLen_; }
  int shortHits13() const { return shortHits13_; }
  double shortSum() const { return shortSum_; }
  double shortSumSq() const { return shortSumSq_; }
  std::optional<double> lastCrash() const { return lastCrash_; }

  /** Access lag ring element by absolute index (0 = oldest). */
  double lagAt(int i) const {
    if (i < 0 || i >= lagLen_) return 0.0;
    const int start = lagLen_ < LAG_CAP ? 0 : lagPos_;
    return lagRing_[(start + i) % LAG_CAP];
  }

  /** Newest lag value (lag_1). */
  double lag1() const {
    if (lagLen_ == 0) return 0.0;
    const int idx = (lagPos_ - 1 + LAG_CAP) % LAG_CAP;
    return lagRing_[idx];
  }

  double lagN(int n) const {
    if (n < 1 || n > lagLen_) return 0.0;
    const int idx = (lagPos_ - n + LAG_CAP) % LAG_CAP;
    return lagRing_[idx];
  }

private:
  int count_ = 0;
  std::optional<double> lastCrash_;
  WelfordState welford_;
  double ewma_ = 1.3;
  double ewmaHit13_ = 0.65;
  HitCounters hits_;
  RunState runs_;
  SinceCounters since_;
  MarkovState markov_;
  std::array<double, LAG_CAP> lagRing_{};
  int lagLen_ = 0;
  int lagPos_ = 0;
  std::array<double, SHORT_CAP> shortRing_{};
  int shortLen_ = 0;
  int shortPos_ = 0;
  double shortSum_ = 0.0;
  double shortSumSq_ = 0.0;
  int shortHits13_ = 0;
  std::optional<int64_t> lastBeganAtMs_;
  double lastGapS_ = 0.0;
  int gapCount_ = 0;
  int64_t updatedAt_ = 0;
};

} // namespace pe
