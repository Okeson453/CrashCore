#pragma once

/**
 * FeatureEngineV2 — assembles all O(1) incremental feature families
 * with per-family try/catch → featureStage tagging.
 * Ported from feature-engine-v2.ts
 */

#include "pe/features/cross_target.hpp"
#include "pe/features/entropy.hpp"
#include "pe/features/feature_meta.hpp"
#include "pe/features/gap.hpp"
#include "pe/features/lag.hpp"
#include "pe/features/markov.hpp"
#include "pe/features/run.hpp"
#include "pe/features/spectral.hpp"
#include "pe/features/time.hpp"
#include "pe/state/incremental_state_engine.hpp"
#include "pe/types.hpp"
#include "pe/util/logger.hpp"
#include "pe/util/time.hpp"
#include "pe/util/uuid.hpp"

#include <string>
#include <unordered_map>
#include <vector>

namespace pe {

class FeatureEngineV2 {
public:
  FeatureVector compute(const IncrementalStateEngine& engine,
                        const std::string& roundId = "") const {
    FeatureVector fv;
    fv.roundId = roundId.empty() ? util::randomUUID() : roundId;
    fv.timestamp = util::nowIso8601();
    fv.featureVersion = FEATURE_VERSION_V2;
    fv.meta.sampleSize = engine.count();
    fv.meta.dataQualityScore = qualityScore(engine);
    fv.meta.missingFeatureCount = 0;
    // Pre-size to avoid rehash on the hot path (~55 keys)
    fv.values.reserve(64);

    auto merge = [&](const std::unordered_map<std::string, double>& part) {
      for (const auto& [k, v] : part) {
        fv.values[k] = v;
      }
    };

    // Core stats always present
    fv.values["sample_size"] = static_cast<double>(engine.count());
    fv.values["quality_score"] = fv.meta.dataQualityScore;
    fv.values["mean_crash"] = engine.welford().mean;
    fv.values["std_crash"] = engine.std();
    fv.values["ewma_crash"] = engine.ewma();
    fv.values["ewma_hit_13"] = engine.ewmaHit13();
    fv.values["short_mean"] = engine.shortMean();
    fv.values["short_std"] = std::sqrt(engine.shortVariance());
    fv.values["short_hit_13"] = engine.shortHitRate13();
    fv.values["since_13"] = static_cast<double>(engine.roundsSince(1.3));
    fv.values["since_20"] = static_cast<double>(engine.roundsSince(2.0));
    fv.values["since_50"] = static_cast<double>(engine.roundsSince(5.0));
    fv.values["since_100"] = static_cast<double>(engine.roundsSince(10.0));

    const auto windows = engine.hitRateWindows13();
    fv.values["hit_1_30_20"] = windows.w20;
    fv.values["hit_1_30_50"] = windows.w50;
    fv.values["hit_1_30_100"] = windows.w100;
    fv.values["hit_1_30_200"] = windows.w200;

    // Families with isolation
    try { merge(computeLagFeatures(engine)); }
    catch (const std::exception& e) {
      util::warn(std::string("lag features: ") + e.what());
      ++fv.meta.missingFeatureCount;
    }
    try { merge(computeRunFeatures(engine)); }
    catch (const std::exception& e) {
      util::warn(std::string("run features: ") + e.what());
      ++fv.meta.missingFeatureCount;
    }
    try { merge(computeMarkovFeatures(engine)); }
    catch (const std::exception& e) {
      util::warn(std::string("markov features: ") + e.what());
      ++fv.meta.missingFeatureCount;
    }
    try { merge(computeSpectralFeatures(engine)); }
    catch (const std::exception& e) {
      util::warn(std::string("spectral features: ") + e.what());
      ++fv.meta.missingFeatureCount;
    }
    try { merge(computeEntropyFeatures(engine)); }
    catch (const std::exception& e) {
      util::warn(std::string("entropy features: ") + e.what());
      ++fv.meta.missingFeatureCount;
    }
    try { merge(computeTimeFeatures()); }
    catch (const std::exception& e) {
      util::warn(std::string("time features: ") + e.what());
      ++fv.meta.missingFeatureCount;
    }
    try { merge(computeCrossTargetFeatures(engine)); }
    catch (const std::exception& e) {
      util::warn(std::string("cross-target features: ") + e.what());
      ++fv.meta.missingFeatureCount;
    }
    try { merge(computeGapFeatures(engine)); }
    catch (const std::exception& e) {
      util::warn(std::string("gap features: ") + e.what());
      ++fv.meta.missingFeatureCount;
    }

    return fv;
  }

private:
  static double qualityScore(const IncrementalStateEngine& engine) {
    const auto life = engine.getLifecycleState();
    switch (life) {
      case EngineLifecycleState::COLD:       return 0.0;
      case EngineLifecycleState::WARMING:    return 0.2;
      case EngineLifecycleState::WARM:       return 0.6;
      case EngineLifecycleState::PRODUCTION: return 1.0;
      case EngineLifecycleState::DEGRADED:   return 0.3;
    }
    return 0.0;
  }
};

} // namespace pe
