#include "pe/ml/ml_normalizer.hpp"

#include <algorithm>
#include <cmath>

namespace pe::ml {

MLNormalizer::MLNormalizer(std::vector<double> mean, std::vector<double> scale)
  : mean_(std::move(mean)), scale_(std::move(scale)) {
  if (scale_.size() != mean_.size()) scale_.assign(mean_.size(), 1.0);
  for (auto& s : scale_) if (!(s > 0.0) || !std::isfinite(s)) s = 1.0;
}

void MLNormalizer::transform(const double* input, double* output, std::size_t count) const noexcept {
  if (!input || !output) return;
  const std::size_t n = std::min(count, mean_.size());
  for (std::size_t i = 0; i < n; ++i)
    output[i] = (input[i] - mean_[i]) / scale_[i];
  for (std::size_t i = n; i < count; ++i) output[i] = input[i];
}

} // namespace pe::ml
