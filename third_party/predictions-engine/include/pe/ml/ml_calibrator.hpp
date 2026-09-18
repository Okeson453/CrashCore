#pragma once

#include <cstddef>
#include <vector>

namespace pe::ml {

enum class CalibrationMethod { None, Platt, Isotonic };

class MLCalibrator {
public:
  MLCalibrator() = default;

  static MLCalibrator platt(double a, double b);
  static MLCalibrator isotonic(std::vector<double> xBreaks, std::vector<double> yValues);

  double calibrate(double raw_probability) const noexcept;
  CalibrationMethod method() const noexcept { return method_; }

private:
  CalibrationMethod method_ = CalibrationMethod::None;
  double platt_a_ = 1.0;
  double platt_b_ = 0.0;
  std::vector<double> iso_x_;
  std::vector<double> iso_y_;
};

} // namespace pe::ml
