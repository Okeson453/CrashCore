#pragma once
#include "common/types.hpp"
#include "timing/monotonic_clock.hpp"
#include <atomic>
#include <array>

namespace crashcore {

class LatencyTracker {
public:
  enum class Stage : std::uint8_t {
    Parse = 0, State, Ipc, Queue, Prediction, Validation, Outbox, Total, COUNT
  };

  void record(Stage s, std::int64_t micros) noexcept {
    const auto idx = static_cast<std::size_t>(s);
    if (idx >= static_cast<std::size_t>(Stage::COUNT)) return;
    count_[idx].fetch_add(1, std::memory_order_relaxed);
    sum_[idx].fetch_add(micros, std::memory_order_relaxed);
    auto cur = max_[idx].load(std::memory_order_relaxed);
    while (micros > cur && !max_[idx].compare_exchange_weak(cur, micros, std::memory_order_relaxed)) {}
    buckets_[idx][bucketIndex(micros)].fetch_add(1, std::memory_order_relaxed);
  }

  struct Stats {
    std::uint64_t count = 0;
    std::int64_t  sumUs = 0;
    std::int64_t  maxUs = 0;
    double        avgUs = 0.0;
  };

  Stats stats(Stage s) const noexcept {
    const auto idx = static_cast<std::size_t>(s);
    Stats st;
    st.count = count_[idx].load(std::memory_order_relaxed);
    st.sumUs = sum_[idx].load(std::memory_order_relaxed);
    st.maxUs = max_[idx].load(std::memory_order_relaxed);
    if (st.count > 0) st.avgUs = static_cast<double>(st.sumUs) / static_cast<double>(st.count);
    return st;
  }

  void reset() noexcept {
    for (std::size_t i = 0; i < static_cast<std::size_t>(Stage::COUNT); ++i) {
      count_[i].store(0); sum_[i].store(0); max_[i].store(0);
      for (auto& b : buckets_[i]) b.store(0);
    }
  }

private:
  static constexpr std::size_t NUM_BUCKETS = 16;
  static std::size_t bucketIndex(std::int64_t us) noexcept {
    if (us < 10) return 0;
    if (us < 25) return 1;
    if (us < 50) return 2;
    if (us < 100) return 3;
    if (us < 200) return 4;
    if (us < 500) return 5;
    if (us < 1000) return 6;
    if (us < 2000) return 7;
    if (us < 5000) return 8;
    if (us < 10000) return 9;
    if (us < 25000) return 10;
    if (us < 50000) return 11;
    if (us < 100000) return 12;
    if (us < 250000) return 13;
    if (us < 1000000) return 14;
    return 15;
  }

  std::array<std::atomic<std::uint64_t>, static_cast<std::size_t>(Stage::COUNT)> count_{};
  std::array<std::atomic<std::int64_t>, static_cast<std::size_t>(Stage::COUNT)> sum_{};
  std::array<std::atomic<std::int64_t>, static_cast<std::size_t>(Stage::COUNT)> max_{};
  std::array<std::array<std::atomic<std::uint64_t>, NUM_BUCKETS>, static_cast<std::size_t>(Stage::COUNT)> buckets_{};
};

class ScopedLatency {
public:
  ScopedLatency(LatencyTracker& t, LatencyTracker::Stage s)
      : tracker_(t), stage_(s), start_(MonotonicClock::now()) {}
  ~ScopedLatency() {
    tracker_.record(stage_, MonotonicClock::elapsedUs(start_, MonotonicClock::now()));
  }
  ScopedLatency(const ScopedLatency&) = delete;
  ScopedLatency& operator=(const ScopedLatency&) = delete;
private:
  LatencyTracker& tracker_;
  LatencyTracker::Stage stage_;
  MonotonicClock::time_point start_;
};

} // namespace crashcore
