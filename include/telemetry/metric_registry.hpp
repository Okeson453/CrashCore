#pragma once
/**
 * Lock-free-friendly metric registry.
 * Counters use per-name sharded atomics; gauges/histograms use fine-grained locks.
 * Phase 0.14 fix: replace global mutex on hot-path counterInc.
 */
#include "common/enums.hpp"
#include "timing/timestamp.hpp"
#include <atomic>
#include <mutex>
#include <shared_mutex>
#include <string>
#include <unordered_map>
#include <vector>
#include <cstdint>
#include <memory>
#include <new>

namespace crashcore {

struct alignas(64) AtomicCounter {
  std::atomic<std::uint64_t> v{0};
};

class MetricRegistry {
public:
  void counterInc(const std::string& name, std::uint64_t n = 1) {
    auto* c = ensureCounter(name);
    c->v.fetch_add(n, std::memory_order_relaxed);
  }

  std::uint64_t counterGet(const std::string& name) const {
    std::shared_lock lk(counter_mu_);
    auto it = counters_.find(name);
    if (it == counters_.end()) return 0;
    return it->second->v.load(std::memory_order_relaxed);
  }

  void gaugeSet(const std::string& name, double v) {
    std::lock_guard lk(gauge_mu_);
    gauges_[name] = v;
  }

  double gaugeGet(const std::string& name) const {
    std::lock_guard lk(gauge_mu_);
    auto it = gauges_.find(name);
    return it == gauges_.end() ? 0.0 : it->second;
  }

  void histogramObserve(const std::string& name, double value) {
    std::lock_guard lk(hist_mu_);
    auto& h = histograms_[name];
    h.values.push_back(value);
    if (h.values.size() > 512) h.values.erase(h.values.begin());
    h.sum += value;
    h.count += 1;
    if (value > h.max) h.max = value;
  }

  struct HistStats { std::uint64_t count=0; double sum=0; double max=0; double avg=0; };
  HistStats histogramStats(const std::string& name) const {
    std::lock_guard lk(hist_mu_);
    HistStats s;
    auto it = histograms_.find(name);
    if (it == histograms_.end()) return s;
    s.count = it->second.count; s.sum = it->second.sum; s.max = it->second.max;
    if (s.count) s.avg = s.sum / static_cast<double>(s.count);
    return s;
  }

  std::vector<std::string> counterNames() const {
    std::shared_lock lk(counter_mu_);
    std::vector<std::string> out;
    out.reserve(counters_.size());
    for (const auto& [k, _] : counters_) out.push_back(k);
    return out;
  }

  void reset() {
    {
      std::unique_lock lk(counter_mu_);
      counters_.clear();
    }
    {
      std::lock_guard lk(gauge_mu_);
      gauges_.clear();
    }
    {
      std::lock_guard lk(hist_mu_);
      histograms_.clear();
    }
  }

private:
  AtomicCounter* ensureCounter(const std::string& name) {
    {
      std::shared_lock lk(counter_mu_);
      auto it = counters_.find(name);
      if (it != counters_.end()) return it->second.get();
    }
    std::unique_lock lk(counter_mu_);
    auto it = counters_.find(name);
    if (it != counters_.end()) return it->second.get();
    auto ptr = std::make_unique<AtomicCounter>();
    auto* raw = ptr.get();
    counters_.emplace(name, std::move(ptr));
    return raw;
  }

  struct Hist { std::vector<double> values; double sum=0; double max=0; std::uint64_t count=0; };

  mutable std::shared_mutex counter_mu_;
  std::unordered_map<std::string, std::unique_ptr<AtomicCounter>> counters_;

  mutable std::mutex gauge_mu_;
  std::unordered_map<std::string, double> gauges_;

  mutable std::mutex hist_mu_;
  std::unordered_map<std::string, Hist> histograms_;
};

inline constexpr const char* METRIC_EVENTS_RECEIVED = "events_received";
inline constexpr const char* METRIC_EVENTS_ACCEPTED = "events_accepted";
inline constexpr const char* METRIC_EVENTS_DECODED = "events_decoded";
inline constexpr const char* METRIC_EVENTS_ROUTED = "events_routed";
inline constexpr const char* METRIC_EVENTS_DROPPED = "events_dropped";
inline constexpr const char* METRIC_EVENTS_DUPLICATE = "events_duplicate";
inline constexpr const char* METRIC_EVENTS_STALE = "events_stale";
inline constexpr const char* METRIC_EVENTS_INVALID = "events_invalid";
inline constexpr const char* METRIC_FRAMES_RX = "frames_rx";
inline constexpr const char* METRIC_FRAMES_TX = "frames_tx";
inline constexpr const char* METRIC_PREDICTIONS_SUBMITTED = "predictions_submitted";
inline constexpr const char* METRIC_PREDICTIONS_SKIPPED = "predictions_skipped";
inline constexpr const char* METRIC_OUTBOX_PUBLISHED = "outbox_published";
inline constexpr const char* METRIC_OUTBOX_DELIVERED = "outbox_delivered";
inline constexpr const char* METRIC_OUTBOX_FAILED = "outbox_failed";
inline constexpr const char* METRIC_TELEGRAM_SENT = "telegram_sent";
inline constexpr const char* METRIC_DB_WRITES = "db_writes";
inline constexpr const char* METRIC_RECONNECTS = "reconnects";
inline constexpr const char* METRIC_PREDICTIONS_ACTIONABLE = "predictions_actionable";
inline constexpr const char* METRIC_OUTCOMES_WIN = "outcomes_win";
inline constexpr const char* METRIC_OUTCOMES_LOSS = "outcomes_loss";
inline constexpr const char* METRIC_TELEGRAM_FAILED = "telegram_failed";


} // namespace crashcore
