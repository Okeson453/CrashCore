#include "pe/state/incremental_state_engine.hpp"
#include "pe/util/time.hpp"

#include <algorithm>
#include <cmath>

namespace pe {

void IncrementalStateEngine::seed(const std::vector<double>& crashPoints) {
  reset();
  for (double cp : crashPoints) {
    update(cp);
  }
}

void IncrementalStateEngine::recordBeganAt(int64_t beganAtMs) {
  if (!std::isfinite(static_cast<double>(beganAtMs)) || beganAtMs <= 0) return;
  if (lastBeganAtMs_.has_value() && beganAtMs > *lastBeganAtMs_) {
    const double gapS = static_cast<double>(beganAtMs - *lastBeganAtMs_) / 1000.0;
    if (gapS > 0.0 && gapS <= 600.0) {
      lastGapS_ = gapS;
      gapCount_ += 1;
    }
  }
  lastBeganAtMs_ = beganAtMs;
}

IncrementalStateEngine::GapState IncrementalStateEngine::getGapState() const {
  return {lastGapS_, gapCount_, lastBeganAtMs_};
}

void IncrementalStateEngine::reset() {
  count_ = 0;
  lastCrash_ = std::nullopt;
  welford_ = {};
  ewma_ = 1.3;
  ewmaHit13_ = 0.65;
  hits_ = {};
  runs_ = {};
  since_ = {};
  markov_ = {};
  lagRing_.fill(0.0);
  lagLen_ = 0;
  lagPos_ = 0;
  shortRing_.fill(0.0);
  shortLen_ = 0;
  shortPos_ = 0;
  shortSum_ = 0.0;
  shortSumSq_ = 0.0;
  shortHits13_ = 0;
  updatedAt_ = 0;
  lastBeganAtMs_ = std::nullopt;
  lastGapS_ = 0.0;
  gapCount_ = 0;
}

IncrementalEngineSnapshot IncrementalStateEngine::update(double crashPoint) {
  if (!std::isfinite(crashPoint) || crashPoint <= 0.0) {
    return snapshot();
  }

  const bool above13 = crashPoint >= 1.3;
  const bool above20 = crashPoint >= 2.0;
  const bool above50 = crashPoint >= 5.0;
  const bool above100 = crashPoint >= 10.0;

  // Welford
  auto& w = welford_;
  w.n += 1;
  const double delta = crashPoint - w.mean;
  w.mean += delta / w.n;
  const double delta2 = crashPoint - w.mean;
  w.m2 += delta * delta2;

  // EWMA
  ewma_ = EWMA_ALPHA * crashPoint + (1.0 - EWMA_ALPHA) * ewma_;
  ewmaHit13_ = EWMA_ALPHA * (above13 ? 1.0 : 0.0) + (1.0 - EWMA_ALPHA) * ewmaHit13_;

  // Hits
  if (above13) hits_.t13 += 1;
  if (above20) hits_.t20 += 1;
  if (above50) hits_.t50 += 1;
  if (above100) hits_.t100 += 1;

  // Rounds-since-last-hit
  since_.t13 += 1;
  since_.t20 += 1;
  since_.t50 += 1;
  since_.t100 += 1;
  if (above13) since_.t13 = 0;
  if (above20) since_.t20 = 0;
  if (above50) since_.t50 = 0;
  if (above100) since_.t100 = 0;

  // Runs
  auto& r = runs_;
  if (above13) {
    r.above13 += 1;
    r.below13 = 0;
    if (r.above13 > r.maxAbove13) r.maxAbove13 = r.above13;
  } else {
    r.below13 += 1;
    r.above13 = 0;
    if (r.below13 > r.maxBelow13) r.maxBelow13 = r.below13;
  }
  if (crashPoint < 1.5) r.below15 += 1;
  else r.below15 = 0;
  if (above20) {
    r.above20 += 1;
    r.below20 = 0;
  } else {
    r.below20 += 1;
    r.above20 = 0;
    if (r.below20 > r.maxBelow20) r.maxBelow20 = r.below20;
  }

  // Markov 2-state
  if (markov_.lastAbove13.has_value()) {
    const int from = *markov_.lastAbove13 ? 1 : 0;
    const int to = above13 ? 1 : 0;
    markov_.trans[from][to] += 1;
  }
  markov_.lastAbove13 = above13;

  // Lag ring
  lagRing_[lagPos_] = crashPoint;
  lagPos_ = (lagPos_ + 1) % LAG_CAP;
  if (lagLen_ < LAG_CAP) lagLen_ += 1;

  // Short window ring
  if (shortLen_ == SHORT_CAP) {
    const double evicted = shortRing_[shortPos_];
    shortSum_ -= evicted;
    shortSumSq_ -= evicted * evicted;
    if (evicted >= 1.3) shortHits13_ -= 1;
  } else {
    shortLen_ += 1;
  }
  shortRing_[shortPos_] = crashPoint;
  shortPos_ = (shortPos_ + 1) % SHORT_CAP;
  shortSum_ += crashPoint;
  shortSumSq_ += crashPoint * crashPoint;
  if (above13) shortHits13_ += 1;

  count_ += 1;
  lastCrash_ = crashPoint;
  updatedAt_ = util::nowMs();
  return snapshot();
}

IncrementalEngineSnapshot IncrementalStateEngine::snapshot() const {
  IncrementalEngineSnapshot s;
  s.count = count_;
  s.lastCrash = lastCrash_;
  s.welford = welford_;
  s.ewma = ewma_;
  s.ewmaHit13 = ewmaHit13_;
  s.hits = hits_;
  s.runs = runs_;
  s.since = since_;
  s.markov.trans = markov_.trans;
  s.markov.lastAbove13 = markov_.lastAbove13;
  s.lagRing = getLagArray();
  s.lagRingSize = lagLen_;
  s.shortSum = shortSum_;
  s.shortSumSq = shortSumSq_;
  s.shortHits13 = shortHits13_;
  s.shortCount = shortLen_;
  s.updatedAt = updatedAt_;
  s.featureVersion = featureVersion;
  s.lastGapS = lastGapS_;
  s.gapCount = gapCount_;
  return s;
}

std::vector<double> IncrementalStateEngine::getLagArray() const {
  if (lagLen_ == 0) return {};
  std::vector<double> out(static_cast<size_t>(lagLen_));
  const int start = lagLen_ < LAG_CAP ? 0 : lagPos_;
  for (int i = 0; i < lagLen_; ++i) {
    out[static_cast<size_t>(i)] = lagRing_[(start + i) % LAG_CAP];
  }
  return out;
}

double IncrementalStateEngine::variance() const {
  return welford_.n > 1 ? welford_.m2 / (welford_.n - 1) : 0.0;
}

double IncrementalStateEngine::std() const {
  return std::sqrt(variance());
}

double IncrementalStateEngine::hitRate(double target) const {
  if (count_ == 0) return 0.0;
  if (target <= 1.3) return static_cast<double>(hits_.t13) / count_;
  if (target <= 2.0) return static_cast<double>(hits_.t20) / count_;
  if (target <= 5.0) return static_cast<double>(hits_.t50) / count_;
  return static_cast<double>(hits_.t100) / count_;
}

int IncrementalStateEngine::roundsSince(double target) const {
  if (target <= 1.3) return since_.t13;
  if (target <= 2.0) return since_.t20;
  if (target <= 5.0) return since_.t50;
  return since_.t100;
}

double IncrementalStateEngine::windowHitRate(int window, double target) const {
  const int n = std::min(std::max(0, window), lagLen_);
  if (n <= 0) return 0.0;
  int hits = 0;
  for (int i = 1; i <= n; ++i) {
    const int idx = (lagPos_ - i + LAG_CAP) % LAG_CAP;
    if (lagRing_[idx] >= target) hits += 1;
  }
  return static_cast<double>(hits) / n;
}

IncrementalStateEngine::HitRateWindows13 IncrementalStateEngine::hitRateWindows13() const {
  return {
    windowHitRate(20, 1.3),
    windowHitRate(50, 1.3),
    windowHitRate(100, 1.3),
    windowHitRate(200, 1.3)
  };
}

double IncrementalStateEngine::shortHitRate13() const {
  return shortLen_ > 0 ? static_cast<double>(shortHits13_) / shortLen_ : 0.0;
}

double IncrementalStateEngine::shortMean() const {
  return shortLen_ > 0 ? shortSum_ / shortLen_ : 0.0;
}

double IncrementalStateEngine::shortVariance() const {
  if (shortLen_ < 2) return 0.0;
  const double mean = shortSum_ / shortLen_;
  return std::max(0.0, shortSumSq_ / shortLen_ - mean * mean);
}

double IncrementalStateEngine::markovPNextAbove13() const {
  if (!markov_.lastAbove13.has_value()) return ewmaHit13_;
  const int from = *markov_.lastAbove13 ? 1 : 0;
  const auto& row = markov_.trans[from];
  const int total = row[0] + row[1];
  if (total < 10) return ewmaHit13_;
  return static_cast<double>(row[1]) / total;
}

bool IncrementalStateEngine::isWarm(int minCount) const {
  return count_ >= minCount;
}

EngineLifecycleState IncrementalStateEngine::getLifecycleState() const {
  if (count_ == 0) return EngineLifecycleState::COLD;
  const bool stale = (util::nowMs() - updatedAt_) > DEGRADED_TIMEOUT_MS;
  if (stale) return EngineLifecycleState::DEGRADED;
  if (count_ < 20) return EngineLifecycleState::WARMING;
  if (count_ < 100) return EngineLifecycleState::WARM;
  return EngineLifecycleState::PRODUCTION;
}

std::vector<double> IncrementalStateEngine::getRecentPoints(int max) const {
  const int n = std::min(max, lagLen_);
  if (n <= 0) return {};
  std::vector<double> out(static_cast<size_t>(n));
  const int start = (lagPos_ - lagLen_ + LAG_CAP) % LAG_CAP;
  for (int i = 0; i < n; ++i) {
    out[static_cast<size_t>(i)] = lagRing_[(start + i) % LAG_CAP];
  }
  return out;
}

} // namespace pe
