#pragma once
#include "common/types.hpp"
#include <atomic>
#include <array>
#include <string>

namespace crashcore {

class Metrics {
public:
  enum Counter : std::size_t {
    EventsReceived = 0,
    EventsDecoded,
    EventsRouted,
    EventsDropped,
    PredictionsSubmitted,
    PredictionsActionable,
    OutcomesWin,
    OutcomesLoss,
    OutboxPublished,
    OutboxDelivered,
    OutboxFailed,
    Reconnects,
    COUNT
  };

  void inc(Counter c, std::uint64_t n = 1) noexcept {
    if (c < COUNT) counters_[c].fetch_add(n, std::memory_order_relaxed);
  }

  std::uint64_t get(Counter c) const noexcept {
    return c < COUNT ? counters_[c].load(std::memory_order_relaxed) : 0;
  }

  void reset() noexcept {
    for (auto& c : counters_) c.store(0, std::memory_order_relaxed);
  }

private:
  std::array<std::atomic<std::uint64_t>, COUNT> counters_{};
};

} // namespace crashcore
