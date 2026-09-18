#pragma once
/** Platt scaling calibrator (TE calibration parity subset). */
#include <cmath>
#include <mutex>
#include <vector>
namespace crashcore {
class PlattCalibrator {
public:
  void fit(const std::vector<double>& scores, const std::vector<int>& labels) {
    std::lock_guard lk(mu_);
    // Simplified logistic fit: a,b via mean alignment
    if (scores.empty() || scores.size()!=labels.size()) return;
    double meanS=0, meanY=0;
    for (size_t i=0;i<scores.size();++i){ meanS+=scores[i]; meanY+=labels[i]; }
    meanS/=scores.size(); meanY/=labels.size();
    a_ = 1.0; b_ = meanY - meanS;
    fitted_ = true; samples_ = scores.size();
  }
  double calibrate(double score) const {
    std::lock_guard lk(mu_);
    if (!fitted_) return score;
    double z = a_ * score + b_;
    // sigmoid
    if (z > 30) return 1.0; if (z < -30) return 0.0;
    return 1.0 / (1.0 + std::exp(-z));
  }
  bool fitted() const { std::lock_guard lk(mu_); return fitted_; }
  std::size_t samples() const { std::lock_guard lk(mu_); return samples_; }
private:
  mutable std::mutex mu_;
  double a_=1, b_=0; bool fitted_=false; std::size_t samples_=0;
};
} // namespace crashcore
