#pragma once

#include <algorithm>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace pe::ml {

struct LatencyStats {
  double p50 = 0.0;
  double p95 = 0.0;
  double p99 = 0.0;
  double max = 0.0;
  double mean = 0.0;
  std::uint64_t count = 0;
};

class MLMetrics {
public:
  void recordStage(int stage, double us) noexcept; // 0=prep,1=infer,2=cal,3=fuse
  LatencyStats stats(int stage) const;
  void reset() noexcept;

  static constexpr int kPrep = 0;
  static constexpr int kInfer = 1;
  static constexpr int kCal = 2;
  static constexpr int kFuse = 3;
  static constexpr int kStages = 4;
  static constexpr std::size_t kRing = 512;

private:
  struct StageRing {
    double buf[kRing]{};
    std::atomic<std::uint64_t> idx{0};
    std::atomic<std::uint64_t> count{0};
  };
  StageRing stages_[kStages];
};

} // namespace pe::ml
