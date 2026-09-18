#pragma once

namespace pe::ml {

struct FusionInput {
  double statistical_probability = 0.0;
  double ml_probability = 0.0;
  double statistical_confidence = 0.0;
  double ml_confidence = 0.0;
  double disagreement = 0.0;
  double regime_confidence = 0.0;
};

struct FusionResult {
  double probability = 0.0;
  double confidence = 0.0;
};

class MLFusion {
public:
  /** Logistic blend coefficients: [bias, stat_p, ml_p, disagreement, regime_conf]. */
  explicit MLFusion(double w0 = 0.0, double w_stat = 1.0, double w_ml = 0.0,
                    double w_disagree = 0.0, double w_regime = 0.0)
    : w0_(w0), w_stat_(w_stat), w_ml_(w_ml), w_disagree_(w_disagree), w_regime_(w_regime) {}

  /** Fixed linear weight blend used in CANARY (ml_weight in [0,1]). */
  static MLFusion fixedWeight(double ml_weight);

  FusionResult fuse(const FusionInput& in) const noexcept;

  void setWeights(double w0, double w_stat, double w_ml, double w_disagree, double w_regime) noexcept {
    w0_ = w0; w_stat_ = w_stat; w_ml_ = w_ml; w_disagree_ = w_disagree; w_regime_ = w_regime;
  }

private:
  double w0_ = 0.0;
  double w_stat_ = 1.0;
  double w_ml_ = 0.0;
  double w_disagree_ = 0.0;
  double w_regime_ = 0.0;
};

} // namespace pe::ml
