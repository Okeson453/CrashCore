#include "ingestion/event_decoder.hpp"
#include "ingestion/event_router.hpp"
#include "ingestion/socketio/socketio_protocol.hpp"
#include "validation/live_validator.hpp"
#include "prediction_interface/prediction_client.hpp"
#include "timing/latency_tracker.hpp"
#include "timing/monotonic_clock.hpp"
#include <cstdio>
using namespace crashcore;
int main() {
  LatencyTracker tracker;
  EventDecoder decoder(&tracker);
  EventRouter router(16384, &tracker);
  LiveValidator validator(&tracker);
  PredictionClient pred;
  pred.setResponseHandler([&](const PredictionResponse& resp) {
    if (resp.decision == PredictionDecision::Enter)
      validator.registerPrediction(fromResponse(resp, resp.targetRoundId));
  });
  constexpr int ROUNDS = 1000;
  auto t0 = MonotonicClock::now();
  for (int r = 0; r < ROUNDS; ++r) {
    for (int i = 0; i < 5; ++i) {
      auto frame = encodeEvent("/g/cm", "pg", encodeProgressPayload(i * 1000));
      auto ev = decoder.decodeBinary(frame.data(), frame.size());
      if (ev) {
        ev.value().gameId = "g" + std::to_string(r);
        ev.value().sequence = static_cast<SequenceNum>(i);
        router.route(std::move(ev.value()));
      }
    }
    while (auto e = router.popPrediction()) pred.submit(makePredictionRequest(*e));
    auto endFrame = encodeEvent("/g/cm", "ed", encodeEndPayload((uint64_t)r, 1.5 + (r % 10) * 0.1));
    auto endEv = decoder.decodeBinary(endFrame.data(), endFrame.size());
    if (endEv) {
      endEv.value().gameId = "g" + std::to_string(r);
      endEv.value().roundId = endEv.value().gameId;
      endEv.value().crashPoint = 1.5 + (r % 10) * 0.1;
      router.route(std::move(endEv.value()));
    }
    while (auto e = router.popValidation()) validator.onGameEnd(*e);
  }
  auto totalUs = MonotonicClock::elapsedUs(t0, MonotonicClock::now());
  std::printf("e2e rounds=%d total_us=%lld us_per_round=%.1f\n", ROUNDS,
              (long long)totalUs, (double)totalUs / ROUNDS);
  return 0;
}
