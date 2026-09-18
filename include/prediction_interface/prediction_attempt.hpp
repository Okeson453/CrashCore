#pragma once
/**
 * PORT_CONTRACT from TestingEngine live/prediction-attempt.ts
 * Coordinates attempt identity, fencing, timeouts — no model inference.
 */
#include "prediction_interface/prediction_request.hpp"
#include "prediction_interface/prediction_response.hpp"
#include "prediction_interface/prediction_client.hpp"
#include "validation/fencing.hpp"
#include "timing/timestamp.hpp"
#include "timing/latency_tracker.hpp"
#include "common/result.hpp"
#include <atomic>
#include <functional>
#include <mutex>
#include <string>
#include <unordered_map>
#include <optional>
#include <chrono>

namespace crashcore {

enum class AttemptState : std::uint8_t {
  Idle = 0,
  InFlight,
  Completed,
  TimedOut,
  Fenced,
  Failed
};

struct PredictionAttemptRecord {
  std::string attemptId;
  std::string targetRoundId;
  std::string correlationId;
  std::uint64_t generation = 0;
  AttemptState state = AttemptState::Idle;
  TimestampMs startedAtMs = 0;
  TimestampMs completedAtMs = 0;
  PredictionResponse response;
  std::string error;
};

class PredictionAttemptCoordinator {
public:
  using CompleteHandler = std::function<void(const PredictionAttemptRecord&)>;

  explicit PredictionAttemptCoordinator(PredictionClient& client,
                                        GenerationFence& fence,
                                        LatencyTracker* tracker = nullptr,
                                        std::int64_t timeoutMs = 50)
      : client_(client), fence_(fence), tracker_(tracker), timeout_ms_(timeoutMs) {}

  void setCompleteHandler(CompleteHandler h) { on_complete_ = std::move(h); }

  Result<std::string> begin(const PredictionRequest& req) {
    const auto gen = fence_.bump(req.targetRoundId);
    PredictionAttemptRecord rec;
    rec.attemptId = req.correlationId.empty()
        ? (req.targetRoundId + "-" + std::to_string(nowMs()))
        : req.correlationId;
    rec.targetRoundId = req.targetRoundId;
    rec.correlationId = rec.attemptId;
    rec.generation = gen;
    rec.state = AttemptState::InFlight;
    rec.startedAtMs = nowMs();

    {
      std::lock_guard lk(mu_);
      attempts_[rec.attemptId] = rec;
    }

    PredictionRequest r = req;
    r.correlationId = rec.attemptId;
    const auto t0 = steadyNs();
    auto submit = client_.submit(r);
    if (tracker_) tracker_->record(LatencyTracker::Stage::Ipc, (steadyNs() - t0) / 1000);
    if (!submit) {
      std::lock_guard lk(mu_);
      auto& a = attempts_[rec.attemptId];
      a.state = AttemptState::Failed;
      a.error = submit.error().message;
      a.completedAtMs = nowMs();
      ++failed_;
      return submit.error();
    }
    ++started_;
    return rec.attemptId;
  }

  /** Apply response if generation still current. */
  bool complete(const PredictionResponse& resp) {
    std::lock_guard lk(mu_);
    auto it = attempts_.find(resp.correlationId);
    if (it == attempts_.end()) {
      // try match by target
      for (auto& [id, a] : attempts_) {
        if (a.targetRoundId == resp.targetRoundId && a.state == AttemptState::InFlight) {
          return completeLocked(a, resp);
        }
      }
      return false;
    }
    return completeLocked(it->second, resp);
  }

  std::size_t expireTimeouts() {
    const auto now = nowMs();
    std::size_t n = 0;
    std::vector<PredictionAttemptRecord> done;
    {
      std::lock_guard lk(mu_);
      for (auto& [id, a] : attempts_) {
        if (a.state == AttemptState::InFlight &&
            now - a.startedAtMs > timeout_ms_) {
          a.state = AttemptState::TimedOut;
          a.completedAtMs = now;
          ++timeouts_;
          ++n;
          done.push_back(a);
        }
      }
    }
    for (auto& a : done) if (on_complete_) on_complete_(a);
    return n;
  }

  std::optional<PredictionAttemptRecord> get(const std::string& attemptId) const {
    std::lock_guard lk(mu_);
    auto it = attempts_.find(attemptId);
    if (it == attempts_.end()) return std::nullopt;
    return it->second;
  }

  std::uint64_t startedCount() const noexcept { return started_.load(); }
  std::uint64_t completedCount() const noexcept { return completed_.load(); }
  std::uint64_t timeoutCount() const noexcept { return timeouts_.load(); }
  std::uint64_t fencedCount() const noexcept { return fenced_.load(); }
  std::uint64_t failedCount() const noexcept { return failed_.load(); }

private:
  bool completeLocked(PredictionAttemptRecord& a, const PredictionResponse& resp) {
    if (a.state != AttemptState::InFlight) return false;
    if (!fence_.isCurrent(a.targetRoundId, a.generation)) {
      a.state = AttemptState::Fenced;
      a.completedAtMs = nowMs();
      ++fenced_;
      if (on_complete_) on_complete_(a);
      return false;
    }
    a.response = resp;
    a.state = AttemptState::Completed;
    a.completedAtMs = nowMs();
    ++completed_;
    if (on_complete_) on_complete_(a);
    return true;
  }

  PredictionClient& client_;
  GenerationFence& fence_;
  LatencyTracker* tracker_ = nullptr;
  std::int64_t timeout_ms_;
  CompleteHandler on_complete_;
  mutable std::mutex mu_;
  std::unordered_map<std::string, PredictionAttemptRecord> attempts_;
  std::atomic<std::uint64_t> started_{0}, completed_{0}, timeouts_{0};
  std::atomic<std::uint64_t> fenced_{0}, failed_{0};
};

} // namespace crashcore
