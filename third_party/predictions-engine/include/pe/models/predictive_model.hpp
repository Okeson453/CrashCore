#pragma once

#include "pe/types.hpp"

#include <memory>
#include <string>

namespace pe {

/**
 * Model capability contract.
 * Baseline models may no-op fit(); trainable models implement fit().
 */
class PredictiveModel {
public:
  virtual ~PredictiveModel() = default;
  virtual const ModelIdentity& identity() const = 0;
  virtual void fit(const Dataset& /*trainingData*/) {} // optional
  virtual PredictionOutput predict(
      const FeatureVector& features,
      ThresholdTarget target,
      const Regime* regime) const = 0;
  virtual void observeOutcome(
      double /*predicted*/,
      int /*actual*/, // 0 | 1
      double /*crashPoint*/ = 0.0,
      ThresholdTarget /*target*/ = 1.3) {}
};

} // namespace pe
