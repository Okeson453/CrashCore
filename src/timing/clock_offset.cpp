#include "timing/clock_offset.hpp"

namespace crashcore {

const char* kClockOffsetModule = "crashcore.timing.clock_offset";

std::int64_t clockOffsetMsSafe(const ClockOffset& off) {
  return off.offsetMs();
}

bool clockOffsetSkewAlarm(const ClockOffset& off, std::int64_t thresholdMs) {
  return off.skewAlarm(thresholdMs);
}

} // namespace crashcore
