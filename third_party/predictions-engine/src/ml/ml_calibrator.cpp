#include "pe/ml/ml_calibrator.hpp"

#include <algorithm>
#include <cmath>

namespace pe::ml {

MLCalibrator MLCalibrator::platt(double a, double b) {
  MLCalibrator c;
  c.method_ = CalibrationMethod::Platt;
  c.platt_a_ = a;
  c.platt_b_ = b;
  return c;
}

MLCalibrator MLCalibrator::isotonic(std::vector<double> xBreaks, std::vector<double> yValues) {
  MLCalibrator c;
  c.method_ = CalibrationMethod::Isotonic;
  c.iso_x_ = std::move(xBreaks);
  c.iso_y_ = std::move(yValues);
  return c;
}

double MLCalibrator::calibrate(double raw_probability) const noexcept {
  if (!std::isfinite(raw_probability)) return 0.5;
  const double p = std::min(0.999, std::max(0.001, raw_probability));

  if (method_ == CalibrationMethod::Platt) {
    // Platt: 1 / (1 + exp(a * logit(p) + b)); use score as logit approx via log-odds
    const double logit = std::log(p / (1.0 - p));
    const double z = platt_a_ * logit + platt_b_;
    double out;
    if (z >= 0.0) {
      const double e = std::exp(-z);
      out = 1.0 / (1.0 + e);
    } else {
      const double e = std::exp(z);
      out = e / (1.0 + e);
    }
    return std::min(0.99, std::max(0.01, out));
  }

  if (method_ == CalibrationMethod::Isotonic && !iso_x_.empty() && iso_x_.size() == iso_y_.size()) {
    // Piecewise-constant / linear interpolation on sorted breaks
    if (p <= iso_x_.front()) return iso_y_.front();
    if (p >= iso_x_.back()) return iso_y_.back();
    for (std::size_t i = 1; i < iso_x_.size(); ++i) {
      if (p <= iso_x_[i]) {
        const double t = (p - iso_x_[i - 1]) / (iso_x_[i] - iso_x_[i - 1] + 1e-15);
        return iso_y_[i - 1] + t * (iso_y_[i] - iso_y_[i - 1]);
      }
    }
  }

  return p;
}

} // namespace pe::ml
