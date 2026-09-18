#pragma once

/**
 * In-memory logistic regression MLModel — pure math, no I/O on predict.
 * Coefficients loaded once by ml_model_loader.
 */

#include "pe/ml/ml_model.hpp"

#include <cmath>
#include <vector>

namespace pe::ml {

class MLLogisticModel final : public MLModel {
public:
  MLLogisticModel(MLModelMetadata meta,
                  std::vector<double> weights,
                  double bias)
    : meta_(std::move(meta)), weights_(std::move(weights)), bias_(bias) {
    meta_.feature_count = weights_.size();
  }

  MLInferenceResult predict(const MLFeatureVector& features) const noexcept override {
    MLInferenceResult r;
    if (!features.data || features.size != weights_.size()) return r;
    double logit = bias_;
    for (std::size_t i = 0; i < weights_.size(); ++i)
      logit += weights_[i] * features.data[i];
    // numerically stable sigmoid
    double p;
    if (logit >= 0.0) {
      const double e = std::exp(-logit);
      p = 1.0 / (1.0 + e);
    } else {
      const double e = std::exp(logit);
      p = e / (1.0 + e);
    }
    if (!std::isfinite(p)) return r;
    r.raw_score = logit;
    r.probability = std::min(0.99, std::max(0.01, p));
    r.valid = true;
    return r;
  }

  std::size_t featureCount() const noexcept override { return weights_.size(); }
  const MLModelMetadata& metadata() const noexcept override { return meta_; }

  const std::vector<double>& weights() const noexcept { return weights_; }
  double bias() const noexcept { return bias_; }

private:
  MLModelMetadata meta_;
  std::vector<double> weights_;
  double bias_ = 0.0;
};

} // namespace pe::ml
