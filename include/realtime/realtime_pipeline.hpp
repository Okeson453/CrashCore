#pragma once
/**
 * PORT_BEHAVIOR from TestingEngine src/lib/realtime/realtime-pipeline.ts
 * observe(): normalize → validate → metrics; health tracking.
 */
#include "realtime/realtime_types.hpp"
#include "realtime/normalizer.hpp"
#include "realtime/event_validator.hpp"
#include "realtime/realtime_metrics.hpp"
#include "ingestion/crash_event.hpp"
#include "timing/timestamp.hpp"
#include "timing/monotonic_clock.hpp"
#include <functional>
#include <mutex>
#include <string>

namespace crashcore::realtime {

class RealtimePipeline {
public:
  using AcceptedHandler = std::function<void(const ValidatedRoundEvent&)>;

  RealtimePipeline() : validator_(), metrics_() {
    health_.sourceId = "native-bc-socket";
    health_.status = ConnectionStatus::Stopped;
  }

  void setAcceptedHandler(AcceptedHandler h) { on_accepted_ = std::move(h); }

  void setStatus(ConnectionStatus status, std::string detail = {}) {
    std::lock_guard lk(mu_);
    health_.status = status;
    if (!detail.empty()) health_.lastError = std::move(detail);
    if (status == ConnectionStatus::Reconnecting) {
      health_.reconnectAttempts += 1;
      health_.totalReconnects += 1;
      metrics_.markReconnect();
    }
  }

  /** Feed a CrashEvent through normalize → validate. */
  std::optional<ValidatedRoundEvent> observe(const CrashEvent& ev) {
    const auto t0 = MonotonicClock::now();
    auto raw = fromCrashEvent(ev);
    auto normalized = normalizeSourceEvent(raw);
    if (!normalized) {
      metrics_.markInvalid();
      return std::nullopt;
    }
    metrics_.markReceived();
    {
      std::lock_guard lk(mu_);
      health_.lastEventAt = normalized->receivedAt;
    }

    auto result = validator_.validate(*normalized);
    if (!result.ok) {
      if (result.reason == "duplicate") metrics_.markDuplicate();
      else if (result.reason == "stale") metrics_.markStale();
      else metrics_.markInvalid();
      return std::nullopt;
    }
    metrics_.markAccepted();
    if (normalized->phase == RoundPhase::End) {
      std::lock_guard lk(mu_);
      health_.lastEventKind = "end";
    }
    const auto us = MonotonicClock::elapsedUs(t0, MonotonicClock::now());
    metrics_.markProcessingMs(static_cast<double>(us) / 1000.0);

    if (on_accepted_) on_accepted_(result.event);
    return result.event;
  }

  void markE2e(TimestampMs receivedAt) {
    metrics_.markE2e(static_cast<double>(nowMs() - receivedAt));
  }

  AdapterHealth health() const {
    std::lock_guard lk(mu_);
    return health_;
  }

  MetricsSnapshot metricsSnapshot() const { return metrics_.snapshot(); }
  RealtimeMetrics& metrics() noexcept { return metrics_; }
  EventValidator& validator() noexcept { return validator_; }

private:
  EventValidator validator_;
  RealtimeMetrics metrics_;
  mutable std::mutex mu_;
  AdapterHealth health_;
  AcceptedHandler on_accepted_;
};

} // namespace crashcore::realtime
