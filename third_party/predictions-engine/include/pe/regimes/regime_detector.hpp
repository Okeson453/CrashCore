#pragma once

/**
 * Regime detector — deterministic classification.
 * Ported from regime-detector.ts
 */

#include "pe/types.hpp"
#include "pe/util/time.hpp"
#include "pe/util/uuid.hpp"

#include <cmath>
#include <string>
#include <vector>

namespace pe {

class RegimeDetector {
public:
  Regime detect(const std::vector<double>& recentCrashPoints,
                const std::string& atTimestamp = {}) const {
    const auto& cps = recentCrashPoints;
    const int n = static_cast<int>(cps.size());
    const int windowN = std::min(n, 50);
    const auto begin = cps.end() - windowN;
    const auto end = cps.end();

    double lowConc = 0.0, highConc = 0.0, sum = 0.0, sumSq = 0.0;
    for (auto it = begin; it != end; ++it) {
      const double c = *it;
      if (c < 1.5) lowConc += 1.0;
      if (c >= 5.0) highConc += 1.0;
      sum += c;
      sumSq += c * c;
    }
    if (windowN > 0) {
      lowConc /= windowN;
      highConc /= windowN;
    }
    const double meanCp = windowN > 0 ? sum / windowN : 0.0;
    const double var = windowN > 1 ? std::max(0.0, sumSq / windowN - meanCp * meanCp) : 0.0;
    const double vol = std::sqrt(var);

    StreakState streakState = StreakState::Neutral;
    const int consecLow = countConsec(cps, [](double c) { return c < 1.3; });
    const int consecHigh = countConsec(cps, [](double c) { return c >= 2.0; });
    if (consecLow >= 5) streakState = StreakState::Low;
    else if (consecHigh >= 3) streakState = StreakState::High;
    else if (consecLow >= 2 && consecHigh >= 1) streakState = StreakState::Mixed;

    auto hit = [&](double thr) {
      if (windowN == 0) return 0.0;
      int h = 0;
      for (auto it = begin; it != end; ++it) if (*it >= thr) ++h;
      return static_cast<double>(h) / windowN;
    };

    const bool anomalyState = windowN < 10 || vol > 20.0 || meanCp > 50.0;

    std::vector<std::string> explanation;
    if (lowConc > 0.7)
      explanation.push_back("High low-multiplier concentration (" +
                            std::to_string(static_cast<int>(lowConc * 100)) + "%)");
    if (highConc > 0.15)
      explanation.push_back("Elevated high-multiplier frequency");
    if (vol > 5.0)
      explanation.push_back("Elevated volatility");
    if (streakState == StreakState::Low)
      explanation.push_back("Consecutive low streak of " + std::to_string(consecLow));
    if (anomalyState)
      explanation.push_back("Anomaly flags active");
    if (explanation.empty())
      explanation.push_back("Neutral regime");

    std::string name = "neutral";
    if (anomalyState) name = "anomalous";
    else if (lowConc > 0.75 && streakState == StreakState::Low) name = "deep-low";
    else if (highConc > 0.2) name = "high-burst";
    else if (streakState == StreakState::High) name = "high-streak";
    else if (streakState == StreakState::Low) name = "low-streak";
    else if (vol > 8.0) name = "volatile";

    Regime r;
    r.id = name;
    r.name = name;
    r.instanceId = util::randomUUID();
    r.dimensions.lowMultiplierConcentration = lowConc;
    r.dimensions.highMultiplierConcentration = highConc;
    r.dimensions.volatility = vol;
    r.dimensions.streakState = streakState;
    r.dimensions.thresholdFrequency = {
      {"1.30", hit(1.3)},
      {"2.00", hit(2.0)},
      {"5.00", hit(5.0)},
      {"10.00", hit(10.0)},
    };
    r.dimensions.anomalyState = anomalyState;
    r.confidence = anomalyState ? 0.4 : (windowN >= 30 ? 0.85 : 0.6);
    r.explanation = std::move(explanation);
    r.detectedAt = atTimestamp.empty() ? util::nowIso8601() : atTimestamp;
    return r;
  }

private:
  template <typename Pred>
  static int countConsec(const std::vector<double>& cps, Pred pred) {
    int count = 0;
    for (auto it = cps.rbegin(); it != cps.rend(); ++it) {
      if (pred(*it)) ++count;
      else break;
    }
    return count;
  }
};

} // namespace pe
