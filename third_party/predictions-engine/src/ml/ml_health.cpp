#include "pe/ml/ml_health.hpp"

namespace pe::ml {

void MLHealth::recordSuccess(double inference_us) noexcept {
  inference_count_.fetch_add(1, std::memory_order_relaxed);
  if (inference_us > 0.0)
    us_sum_.fetch_add(static_cast<std::uint64_t>(inference_us), std::memory_order_relaxed);
}

void MLHealth::recordError() noexcept {
  error_count_.fetch_add(1, std::memory_order_relaxed);
  inference_count_.fetch_add(1, std::memory_order_relaxed);
}

void MLHealth::recordNaN() noexcept {
  nan_count_.fetch_add(1, std::memory_order_relaxed);
  error_count_.fetch_add(1, std::memory_order_relaxed);
  inference_count_.fetch_add(1, std::memory_order_relaxed);
}

void MLHealth::recordFeatureMismatch() noexcept {
  mismatch_count_.fetch_add(1, std::memory_order_relaxed);
  error_count_.fetch_add(1, std::memory_order_relaxed);
  inference_count_.fetch_add(1, std::memory_order_relaxed);
}

bool MLHealth::shouldDegrade(double max_error_rate) const noexcept {
  const auto n = inference_count_.load(std::memory_order_relaxed);
  if (n < 50) return false;
  const auto e = error_count_.load(std::memory_order_relaxed);
  return static_cast<double>(e) / static_cast<double>(n) > max_error_rate;
}

void MLHealth::reset() noexcept {
  inference_count_.store(0, std::memory_order_relaxed);
  error_count_.store(0, std::memory_order_relaxed);
  nan_count_.store(0, std::memory_order_relaxed);
  mismatch_count_.store(0, std::memory_order_relaxed);
  us_sum_.store(0, std::memory_order_relaxed);
}

} // namespace pe::ml
