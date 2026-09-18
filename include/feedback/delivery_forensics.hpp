#pragma once
#include "timing/timestamp.hpp"
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>
#include <atomic>
#include <optional>

namespace crashcore {

enum class DeliveryStage { Published, Claimed, Attempted, Delivered, Failed, DeadLettered };
inline const char* toString(DeliveryStage s) {
  switch (s) {
    case DeliveryStage::Published: return "published";
    case DeliveryStage::Claimed: return "claimed";
    case DeliveryStage::Attempted: return "attempted";
    case DeliveryStage::Delivered: return "delivered";
    case DeliveryStage::Failed: return "failed";
    case DeliveryStage::DeadLettered: return "dead_lettered";
  }
  return "unknown";
}

struct DeliveryTrace {
  std::string correlationId, outboxId, lastError;
  std::vector<std::pair<DeliveryStage, TimestampMs>> stages;
};

class DeliveryForensics {
public:
  void note(const std::string& correlationId, DeliveryStage stage,
            std::string outboxId = {}, std::string error = {}) {
    std::lock_guard lk(mu_);
    auto& t = traces_[correlationId];
    t.correlationId = correlationId;
    if (!outboxId.empty()) t.outboxId = std::move(outboxId);
    t.stages.emplace_back(stage, nowMs());
    if (!error.empty()) t.lastError = std::move(error);
    ++events_;
    if (traces_.size() > 5000) traces_.erase(traces_.begin());
  }
  std::optional<DeliveryTrace> get(const std::string& correlationId) const {
    std::lock_guard lk(mu_);
    auto it = traces_.find(correlationId);
    if (it == traces_.end()) return std::nullopt;
    return it->second;
  }
  std::vector<DeliveryTrace> findLost(TimestampMs olderThanMs = 60000) const {
    std::lock_guard lk(mu_);
    const auto cutoff = nowMs() - olderThanMs;
    std::vector<DeliveryTrace> lost;
    for (const auto& [_, t] : traces_) {
      bool published = false, delivered = false; TimestampMs pubAt = 0;
      for (const auto& [st, ts] : t.stages) {
        if (st == DeliveryStage::Published) { published = true; pubAt = ts; }
        if (st == DeliveryStage::Delivered) delivered = true;
      }
      if (published && !delivered && pubAt > 0 && pubAt < cutoff) lost.push_back(t);
    }
    return lost;
  }
  std::uint64_t eventCount() const noexcept { return events_.load(); }
private:
  mutable std::mutex mu_;
  std::unordered_map<std::string, DeliveryTrace> traces_;
  std::atomic<std::uint64_t> events_{0};
};

} // namespace crashcore
