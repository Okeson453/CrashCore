#include "pe/ml/ml_preprocessor.hpp"

#include <cmath>

namespace pe::ml {

bool preprocess(const pe::FeatureVector& features,
                double* out,
                std::size_t capacity,
                std::size_t* written) noexcept {
  if (!out || !written) return false;
  const auto& order = mlFeatureOrder();
  if (capacity < order.size()) return false;

  for (std::size_t i = 0; i < order.size(); ++i) {
    double v = 0.0;
    auto it = features.values.find(order[i]);
    if (it != features.values.end()) v = it->second;
    if (!std::isfinite(v)) return false;
    out[i] = v;
  }
  *written = order.size();
  return true;
}

} // namespace pe::ml
