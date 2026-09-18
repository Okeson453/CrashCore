/**
 * CrashCore frame replay tool.
 * Usage: replay [--speed N] [--file path]  (synthetic demo if no file)
 */
#include "tools/replay_source.hpp"
#include "ingestion/event_decoder.hpp"
#include "ingestion/event_router.hpp"
#include "ingestion/crash_feed.hpp"
#include "ingestion/socketio/socketio_protocol.hpp"
#include "validation/validator.hpp"
#include "prediction_interface/prediction_client.hpp"
#include "timing/latency_tracker.hpp"
#include "timing/timestamp.hpp"
#include <cstdio>
#include <thread>
#include <chrono>
#include <cmath>

using namespace crashcore;

static std::vector<RecordedFrame> makeSyntheticRound() {
  std::vector<RecordedFrame> frames;
  TimestampMs t0 = nowMs();
  // start
  {
    auto payload = encodeEndPayload(42, 1.0); // reuse encoder shape for start-like
    auto frame = encodeEvent("/g/cm", "st", payload);
    frames.push_back({t0, frame});
  }
  // progress ticks
  for (int i = 1; i <= 20; ++i) {
    auto payload = encodeProgressPayload(i * 500);
    auto frame = encodeEvent("/g/cm", "pg", payload);
    frames.push_back({t0 + i * 50, frame});
  }
  // end at ~2.5x
  {
    auto payload = encodeEndPayload(42, 2.50, "abc");
    auto frame = encodeEvent("/g/cm", "ed", payload);
    frames.push_back({t0 + 1100, frame});
  }
  return frames;
}

int main(int argc, char** argv) {
  double speed = 1.0;
  std::string file;
  for (int i = 1; i < argc; ++i) {
    std::string a = argv[i];
    if (a == "--speed" && i + 1 < argc) speed = std::atof(argv[++i]);
    if (a == "--file" && i + 1 < argc) file = argv[++i];
  }

  ReplaySource src(file);
  if (file.empty() || !src.load()) {
    std::printf("Using synthetic round replay\n");
    src.loadSynthetic(makeSyntheticRound());
  } else {
    std::printf("Loaded %zu frames from %s\n", src.size(), file.c_str());
  }

  LatencyTracker latency;
  EventDecoder decoder(&latency);
  EventRouter router(4096, &latency);
  CrashFeed feed(decoder, router);
  Validator validator(&latency);
  PredictionClient pred;

  pred.setResponseHandler([&](const PredictionResponse& resp) {
    if (!isActionable(resp)) return;
    auto ev = fromResponse(resp, resp.targetRoundId);
    validator.onPrediction(ev);
  });

  feed.start();
  std::size_t decoded = 0, ends = 0;
  TimestampMs prevTs = 0;

  while (auto frame = src.next()) {
    if (prevTs > 0 && speed > 0) {
      auto delta = frame->timestampMs - prevTs;
      if (delta > 0) {
        auto sleepMs = static_cast<int>(delta / speed);
        if (sleepMs > 0 && sleepMs < 5000)
          std::this_thread::sleep_for(std::chrono::milliseconds(sleepMs));
      }
    }
    prevTs = frame->timestampMs;
    auto r = feed.onFrame(frame->data.data(), frame->data.size());
    if (r) {
      ++decoded;
      if (r.value().kind == EventKind::End) ++ends;
    }
    // Drain
    while (auto ev = router.popPrediction()) {
      pred.submit(makePredictionRequest(*ev));
    }
    while (auto ev = router.popValidation()) {
      auto outs = validator.onRoundEnd(*ev);
      for (const auto& o : outs) {
        std::printf("outcome %s actual=%.2f target=%.2f\n",
                    outcomeName(o.result), o.actualMult, o.targetMult);
      }
    }
  }

  feed.stop();
  auto parseStats = latency.stats(LatencyTracker::Stage::Parse);
  std::printf("Replay done: frames=%zu decoded=%zu ends=%zu parse_avg_us=%.1f\n",
              src.size(), decoded, ends, parseStats.avgUs);
  return 0;
}
