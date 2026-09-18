#pragma once
/** Platt scaling calibrator (TE calibration parity subset). */
#include <algorithm>
#include <cmath>
#include <mutex>
#include <vector>
namespace crashcore {
class PlattCalibrator {
public:
  void fit(const std::vector<double>& scores, const std::vector<int>& labels) {
    std::lock_guard lk(mu_);
    if (scores.empty() || scores.size() != labels.size()) return;

    // Regularized Newton fit for y ~ sigmoid(a * score + b). The previous
    // mean-alignment shortcut was not Platt scaling and could be badly
    // miscalibrated whenever score variance differed from label variance.
    double a = 0.0;
    double b = 0.0;
    constexpr double ridge = 1e-6;
    constexpr int maxIterations = 50;
    for (int iteration = 0; iteration < maxIterations; ++iteration) {
      double gA = ridge * a;
      double gB = ridge * b;
      double hAA = ridge;
      double hAB = 0.0;
      double hBB = ridge;
      for (size_t i = 0; i < scores.size(); ++i) {
        const double z = std::clamp(a * scores[i] + b, -30.0, 30.0);
        const double p = 1.0 / (1.0 + std::exp(-z));
        const double error = p - static_cast<double>(labels[i]);
        const double weight = std::max(1e-9, p * (1.0 - p));
        gA += error * scores[i];
        gB += error;
        hAA += weight * scores[i] * scores[i];
        hAB += weight * scores[i];
        hBB += weight;
      }
      const double determinant = hAA * hBB - hAB * hAB;
      if (std::abs(determinant) < 1e-12) break;
      const double stepA = (hBB * gA - hAB * gB) / determinant;
      const double stepB = (-hAB * gA + hAA * gB) / determinant;
      a -= stepA;
      b -= stepB;
      if (std::max(std::abs(stepA), std::abs(stepB)) < 1e-8) break;
    }
    a_ = a;
    b_ = b;
    fitted_ = true;
    samples_ = scores.size();
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
