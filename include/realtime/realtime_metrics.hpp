#pragma once
#include <optional>
/**
 * PORT_BEHAVIOR from TestingEngine src/lib/realtime/metrics.ts + observability metrics
 */
#include "realtime/realtime_types.hpp"
#include <atomic>
#include <mutex>
#include <vector>
#include <cmath>

namespace crashcore::realtime {

class RealtimeMetrics {
public:
  void markReceived() { ++received_; }
  void markAccepted() { ++accepted_; }
  void markDuplicate() { ++duplicates_; }
  void markStale() { ++stale_; }
  void markInvalid() { ++invalid_; }
  void markMissed() { ++missed_; }
  void markReconnect() { ++reconnects_; }

  void markProcessingMs(double ms) {
    std::lock_guard lk(mu_);
    proc_samples_.push_back(ms);
    if (proc_samples_.size() > 256) proc_samples_.erase(proc_samples_.begin());
    last_processing_ms_ = ms;
  }
  void markE2e(double ms) {
    last_e2e_ms_ = ms;
  }
  void markArrivalLag(double ms) { last_arrival_lag_ms_ = ms; }
  void markPredictionMs(double ms) {
    last_prediction_ms_ = ms;
    std::lock_guard lk(mu_);
    pred_samples_.push_back(ms);
    if (pred_samples_.size() > 256) pred_samples_.erase(pred_samples_.begin());
  }
  void markDeliveryMs(double ms) {
    last_delivery_ms_ = ms;
    std::lock_guard lk(mu_);
    del_samples_.push_back(ms);
    if (del_samples_.size() > 256) del_samples_.erase(del_samples_.begin());
  }

  MetricsSnapshot snapshot() const {
    MetricsSnapshot s;
    s.eventsReceived = received_.load();
    s.eventsAccepted = accepted_.load();
    s.duplicates = duplicates_.load();
    s.stale = stale_.load();
    s.invalid = invalid_.load();
    s.missed = missed_.load();
    s.reconnects = reconnects_.load();
    if (last_arrival_lag_ms_ >= 0) s.lastArrivalLagMs = last_arrival_lag_ms_;
    if (last_processing_ms_ >= 0) s.lastProcessingMs = last_processing_ms_;
    if (last_prediction_ms_ >= 0) s.lastPredictionMs = last_prediction_ms_;
    if (last_delivery_ms_ >= 0) s.lastDeliveryMs = last_delivery_ms_;
    if (last_e2e_ms_ >= 0) s.lastE2eMs = last_e2e_ms_;
    {
      std::lock_guard lk(mu_);
      s.avgProcessingMs = avg(proc_samples_);
      s.avgPredictionMs = avg(pred_samples_);
      s.avgDeliveryMs = avg(del_samples_);
    }
    return s;
  }

  void reset() {
    received_ = accepted_ = duplicates_ = stale_ = invalid_ = missed_ = reconnects_ = 0;
    std::lock_guard lk(mu_);
    proc_samples_.clear(); pred_samples_.clear(); del_samples_.clear();
  }

private:
  static std::optional<double> avg(const std::vector<double>& v) {
    if (v.empty()) return std::nullopt;
    double s = 0;
    for (double x : v) s += x;
    return s / static_cast<double>(v.size());
  }

  std::atomic<std::uint64_t> received_{0}, accepted_{0}, duplicates_{0};
  std::atomic<std::uint64_t> stale_{0}, invalid_{0}, missed_{0}, reconnects_{0};
  mutable std::mutex mu_;
  std::vector<double> proc_samples_, pred_samples_, del_samples_;
  double last_arrival_lag_ms_ = -1, last_processing_ms_ = -1;
  double last_prediction_ms_ = -1, last_delivery_ms_ = -1, last_e2e_ms_ = -1;
};

} // namespace crashcore::realtime
