#pragma once
#include "common/types.hpp"
#include "timing/timestamp.hpp"

namespace crashcore {

struct HeartbeatState {
  TimestampMs lastPingMs = 0;
  TimestampMs lastPongMs = 0;
  std::uint64_t pingsSent = 0;
  std::uint64_t pongsReceived = 0;
  bool timedOut = false;
};

} // namespace crashcore
