#include "pe/ml/ml_metrics.hpp"

#include <cmath>

namespace pe::ml {

void MLMetrics::recordStage(int stage, double us) noexcept {
  if (stage < 0 || stage >= kStages || !std::isfinite(us)) return;
  auto& s = stages_[stage];
  const auto i = s.idx.fetch_add(1, std::memory_order_relaxed);
  s.buf[i % kRing] = us;
  s.count.fetch_add(1, std::memory_order_relaxed);
}

LatencyStats MLMetrics::stats(int stage) const {
  LatencyStats out;
  if (stage < 0 || stage >= kStages) return out;
  const auto& s = stages_[stage];
  const auto n = s.count.load(std::memory_order_relaxed);
  if (n == 0) return out;
  const std::size_t take = static_cast<std::size_t>(std::min(n, static_cast<std::uint64_t>(kRing)));
  std::vector<double> v;
  v.reserve(take);
  // Approximate: read ring without locking (best-effort snapshot)
  for (std::size_t i = 0; i < take; ++i) v.push_back(s.buf[i]);
  std::sort(v.begin(), v.end());
  out.count = n;
  out.max = v.back();
  double sum = 0.0;
  for (double x : v) sum += x;
  out.mean = sum / static_cast<double>(v.size());
  out.p50 = v[v.size() * 50 / 100];
  out.p95 = v[std::min(v.size() - 1, v.size() * 95 / 100)];
  out.p99 = v[std::min(v.size() - 1, v.size() * 99 / 100)];
  return out;
}

void MLMetrics::reset() noexcept {
  for (auto& s : stages_) {
    s.idx.store(0, std::memory_order_relaxed);
    s.count.store(0, std::memory_order_relaxed);
  }
}

} // namespace pe::ml
