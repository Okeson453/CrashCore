#include "pe/ml/ml_fusion.hpp"

#include <algorithm>
#include <cmath>

namespace pe::ml {

MLFusion MLFusion::fixedWeight(double ml_weight) {
  const double w = std::min(1.0, std::max(0.0, ml_weight));
  // Approximate fixed blend via logistic weights dominated by linear mix
  return MLFusion(0.0, 1.0 - w, w, 0.0, 0.0);
}

FusionResult MLFusion::fuse(const FusionInput& in) const noexcept {
  FusionResult out;
  const double disagree = std::abs(in.statistical_probability - in.ml_probability);
  double p;
  // CANARY fixedWeight: w0==0, w_disagree==0, w_regime==0, w_stat+w_ml==1 → linear blend
  if (std::abs(w0_) < 1e-12 && std::abs(w_disagree_) < 1e-12 && std::abs(w_regime_) < 1e-12
      && std::abs(w_stat_ + w_ml_ - 1.0) < 1e-9) {
    p = w_stat_ * in.statistical_probability + w_ml_ * in.ml_probability;
  } else {
    const double logit =
        w0_
      + w_stat_ * in.statistical_probability
      + w_ml_ * in.ml_probability
      + w_disagree_ * disagree
      + w_regime_ * in.regime_confidence;
    if (logit >= 0.0) {
      const double e = std::exp(-logit);
      p = 1.0 / (1.0 + e);
    } else {
      const double e = std::exp(logit);
      p = e / (1.0 + e);
    }
  }
  out.probability = std::min(0.99, std::max(0.01, p));
  out.confidence = std::min(0.95,
      0.5 * in.statistical_confidence + 0.5 * in.ml_confidence);
  return out;
}

} // namespace pe::ml
