#pragma once

#include <atomic>
#include <cstddef>
#include <cstdint>

namespace pe::ml {

class MLHealth {
public:
  void recordSuccess(double inference_us) noexcept;
  void recordError() noexcept;
  void recordNaN() noexcept;
  void recordFeatureMismatch() noexcept;

  std::uint64_t inferenceCount() const noexcept { return inference_count_.load(std::memory_order_relaxed); }
  std::uint64_t errorCount() const noexcept { return error_count_.load(std::memory_order_relaxed); }
  std::uint64_t nanCount() const noexcept { return nan_count_.load(std::memory_order_relaxed); }
  std::uint64_t mismatchCount() const noexcept { return mismatch_count_.load(std::memory_order_relaxed); }

  /** True if error rate or NaN rate is elevated. */
  bool shouldDegrade(double max_error_rate = 0.05) const noexcept;

  void reset() noexcept;

private:
  std::atomic<std::uint64_t> inference_count_{0};
  std::atomic<std::uint64_t> error_count_{0};
  std::atomic<std::uint64_t> nan_count_{0};
  std::atomic<std::uint64_t> mismatch_count_{0};
  std::atomic<std::uint64_t> us_sum_{0};
};

} // namespace pe::ml
