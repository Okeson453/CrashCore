#pragma once
#include "timing/timestamp.hpp"
#include "common/types.hpp"
#include <atomic>
#include <cstdint>

namespace crashcore {

struct WebSocketMetrics {
  std::atomic<std::uint64_t> frames_rx{0};
  std::atomic<std::uint64_t> frames_tx{0};
  std::atomic<std::uint64_t> bytes_rx{0};
  std::atomic<std::uint64_t> bytes_tx{0};
  std::atomic<std::uint64_t> reconnects{0};
  std::atomic<std::uint64_t> errors{0};
  std::atomic<std::uint64_t> handshake_ok{0};
  std::atomic<std::uint64_t> handshake_fail{0};
  std::atomic<TimestampMs> last_rx_ms{0};
  std::atomic<TimestampMs> last_tx_ms{0};
  std::atomic<TimestampMs> connected_since_ms{0};

  void onRx(std::size_t n) {
    frames_rx.fetch_add(1, std::memory_order_relaxed);
    bytes_rx.fetch_add(n, std::memory_order_relaxed);
    last_rx_ms.store(nowMs(), std::memory_order_relaxed);
  }
  void onTx(std::size_t n) {
    frames_tx.fetch_add(1, std::memory_order_relaxed);
    bytes_tx.fetch_add(n, std::memory_order_relaxed);
    last_tx_ms.store(nowMs(), std::memory_order_relaxed);
  }
};

} // namespace crashcore
