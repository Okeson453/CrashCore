#include "ingestion/event_decoder.hpp"
#include "common/constants.hpp"
#include "ingestion/event_router.hpp"
#include "ingestion/socketio/socketio_protocol.hpp"
#include "timing/latency_tracker.hpp"
#include "timing/monotonic_clock.hpp"
#include <cstdio>
using namespace crashcore;
int main() {
  LatencyTracker tracker;
  EventDecoder decoder(&tracker);
  EventRouter router(8192, &tracker);
  auto payload = encodeProgressPayload(5000);
  auto frame = encodeEvent("/g/cm", "pg", payload);
  constexpr int N = 10000;
  auto t0 = MonotonicClock::now();
  for (int i = 0; i < N; ++i) {
    auto r = decoder.decodeBinary(frame.data(), frame.size());
    if (r) {
      r.value().sequence = static_cast<SequenceNum>(i);
      r.value().gameId = "bench-" + std::to_string(i % 100);
      router.route(std::move(r.value()));
    }
    while (router.popPrediction()) {}
  }
  auto totalUs = MonotonicClock::elapsedUs(t0, MonotonicClock::now());
  auto parse = tracker.stats(LatencyTracker::Stage::Parse);
  std::printf("iterations=%d total_us=%lld per_op_us=%.2f parse_avg_us=%.2f\n", N,
              (long long)totalUs, (double)totalUs / N, parse.avgUs);
  std::printf("BUDGET_PARSE_US=%lld BUDGET_TOTAL_APP_US=%lld\n",
              (long long)constants::BUDGET_PARSE_US,
              (long long)constants::BUDGET_TOTAL_APP_US);
  bool ok = parse.avgUs <= constants::BUDGET_PARSE_US &&
            ((double)totalUs / N) <= constants::BUDGET_TOTAL_APP_US;
  std::printf("latency_budget=%s\n", ok ? "PASS" : "FAIL");
  return ok ? 0 : 1;
}
