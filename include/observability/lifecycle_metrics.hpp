#pragma once
#include "telemetry/metric_registry.hpp"
#include "timing/timestamp.hpp"
#include <atomic>
#include <mutex>
#include <string>
#include <unordered_map>

namespace crashcore {

class LifecycleMetrics {
public:
  explicit LifecycleMetrics(MetricRegistry* reg = nullptr) : reg_(reg) {}
  void noteFrameRx() { inc("lc_frame_rx"); }
  void noteDecoded() { inc("lc_decoded"); }
  void noteRouted() { inc("lc_routed"); }
  void notePredictionSubmitted() { inc("lc_pred_submitted"); }
  void notePredictionSkip() { inc("lc_pred_skip"); }
  void notePredictionEnter() { inc("lc_pred_enter"); }
  void noteGameEnd() { inc("lc_game_end"); }
  void noteOutcomeWin() { inc("lc_outcome_win"); }
  void noteOutcomeLoss() { inc("lc_outcome_loss"); }
  void noteOutboxPublished() { inc("lc_outbox_pub"); }
  void noteOutboxDelivered() { inc("lc_outbox_delivered"); }
  void noteTelegramSent() { inc("lc_telegram_sent"); }
  void noteReconnect() { inc("lc_reconnect"); }
  void noteBootComplete() { inc("lc_boot_complete"); }
  void observeParseUs(std::int64_t us) { hist("lc_parse_us", static_cast<double>(us)); }
  void observeE2eUs(std::int64_t us) { hist("lc_e2e_us", static_cast<double>(us)); }
  void recordLeadTimes(TimestampMs predGen, TimestampMs nextStart) {
    if (predGen <= 0 || nextStart <= 0) return;
    const double lead = static_cast<double>(nextStart - predGen);
    hist("lc_prediction_lead_ms", lead);
    last_lead_ms_.store(lead);
  }
  struct Snapshot {
    std::unordered_map<std::string, std::uint64_t> counters;
    double lastLeadMs = 0;
  };
  Snapshot getLifecycleMetricsSnapshot() const {
    Snapshot s; s.lastLeadMs = last_lead_ms_.load();
    if (reg_) {
      for (const auto& name : reg_->counterNames())
        if (name.rfind("lc_", 0) == 0) s.counters[name] = reg_->counterGet(name);
    } else {
      std::lock_guard lk(mu_); s.counters = local_counters_;
    }
    return s;
  }
private:
  void inc(const char* name, std::uint64_t n = 1) {
    if (reg_) reg_->counterInc(name, n);
    else { std::lock_guard lk(mu_); local_counters_[name] += n; }
  }
  void hist(const char* name, double v) { if (reg_) reg_->histogramObserve(name, v); }
  MetricRegistry* reg_ = nullptr;
  mutable std::mutex mu_;
  std::unordered_map<std::string, std::uint64_t> local_counters_;
  std::atomic<double> last_lead_ms_{0};
};

} // namespace crashcore
