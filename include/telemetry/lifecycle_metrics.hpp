#pragma once
/**
 * PORT_BEHAVIOR from TestingEngine observability/metrics/lifecycle-metrics.ts
 */
#include "telemetry/metric_registry.hpp"
#include "timing/monotonic_clock.hpp"
#include "timing/timestamp.hpp"
#include <atomic>
#include <string>

namespace crashcore {

class LifecycleMetrics {
public:
  explicit LifecycleMetrics(MetricRegistry& reg) : reg_(reg) {}

  void noteFrameRx(std::size_t bytes) {
    reg_.counterInc(METRIC_FRAMES_RX);
    reg_.counterInc(METRIC_BYTES_RX, bytes);
  }
  void noteDecoded() { reg_.counterInc(METRIC_EVENTS_DECODED); }
  void noteRouted() { reg_.counterInc(METRIC_EVENTS_ROUTED); }
  void noteDropped() { reg_.counterInc(METRIC_EVENTS_DROPPED); }
  void notePredictionSubmit() { reg_.counterInc(METRIC_PREDICTIONS_SUBMITTED); }
  void notePredictionActionable() { reg_.counterInc(METRIC_PREDICTIONS_ACTIONABLE); }
  void noteOutcomeWin() { reg_.counterInc(METRIC_OUTCOMES_WIN); }
  void noteOutcomeLoss() { reg_.counterInc(METRIC_OUTCOMES_LOSS); }
  void noteOutboxPublish() { reg_.counterInc(METRIC_OUTBOX_PUBLISHED); }
  void noteOutboxDelivered() { reg_.counterInc(METRIC_OUTBOX_DELIVERED); }
  void noteReconnect() { reg_.counterInc(METRIC_RECONNECTS); }
  void noteEdProcessingUs(std::int64_t us) {
    reg_.histogramObserve(METRIC_LATENCY_VALIDATION_US, static_cast<double>(us));
  }
  void noteE2eUs(std::int64_t us) {
    reg_.histogramObserve(METRIC_LATENCY_E2E_US, static_cast<double>(us));
  }
  void noteParseUs(std::int64_t us) {
    reg_.histogramObserve(METRIC_LATENCY_PARSE_US, static_cast<double>(us));
  }

  MetricRegistry& registry() noexcept { return reg_; }

private:
  MetricRegistry& reg_;
};

/** Scoped timer that records into a histogram on destruction. */
class ScopedLatency {
public:
  ScopedLatency(MetricRegistry& reg, const char* metric)
      : reg_(reg), metric_(metric), t0_(MonotonicClock::now()) {}
  ~ScopedLatency() {
    reg_.histogramObserve(metric_, static_cast<double>(
        MonotonicClock::elapsedUs(t0_, MonotonicClock::now())));
  }
private:
  MetricRegistry& reg_;
  const char* metric_;
  MonotonicClock::TimePoint t0_;
};

} // namespace crashcore
