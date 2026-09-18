#pragma once
/**
 * Client that talks to the external Prediction Engine.
 * Implementations: shared-memory SPSC, Unix socket, or in-process stub for tests.
 * Never embeds model logic.
 */
#include "prediction_interface/prediction_request.hpp"
#include "prediction_interface/prediction_response.hpp"
#include "prediction_interface/prediction_event.hpp"
#include "prediction_interface/prediction_contract.hpp"
#include "concurrency/spsc_queue.hpp"
#include "timing/latency_tracker.hpp"
#include "timing/timestamp.hpp"
#include "common/result.hpp"
#include <atomic>
#include <functional>
#include <memory>
#include <string>
#include <mutex>
#include <unordered_map>

namespace crashcore {

class PredictionClient {
public:
  using ResponseHandler = std::function<void(const PredictionResponse&)>;

  explicit PredictionClient(PredictionContract contract = {}, LatencyTracker* tracker = nullptr)
      : contract_(std::move(contract)), tracker_(tracker) {}

  virtual ~PredictionClient() = default;

  /**
   * Submit a prediction request. Non-blocking.
   * Response arrives via setResponseHandler or pollResponse.
   */
  virtual Result<void> submit(const PredictionRequest& req) {
    // NON-PRODUCTION stub: always Skip. Production must use InProcessPredictionClient
    // or a fully wired external client. Detect via modelVersion == "stub".
    PredictionResponse resp;
    resp.predictionId = "stub-" + req.correlationId;
    resp.targetRoundId = req.targetRoundId;
    resp.decision = PredictionDecision::Skip;
    resp.confidence = 0.0;
    resp.responseTimeMs = nowMs();
    resp.responseNs = steadyNs();
    resp.correlationId = req.correlationId;
    resp.modelVersion = "stub";
    resp.valid = true;
    pending_[req.correlationId] = resp;
    ++submitted_;
    if (handler_) handler_(resp);
    return Result<void>::success();
  }

  void setResponseHandler(ResponseHandler h) { handler_ = std::move(h); }

  std::optional<PredictionResponse> pollResponse(const std::string& correlationId) {
    std::lock_guard lk(mu_);
    auto it = pending_.find(correlationId);
    if (it == pending_.end()) return std::nullopt;
    auto r = it->second;
    pending_.erase(it);
    return r;
  }

  const PredictionContract& contract() const noexcept { return contract_; }

  std::uint64_t submittedCount() const noexcept {
    return submitted_.load(std::memory_order_relaxed);
  }

protected:
  PredictionContract contract_;
  LatencyTracker* tracker_ = nullptr;
  ResponseHandler handler_;
  std::mutex mu_;
  std::unordered_map<std::string, PredictionResponse> pending_;
  std::atomic<std::uint64_t> submitted_{0};
};

/**
 * Shared-memory oriented client: requests go to an SPSC queue consumed by
 * the external process; responses arrive on a reverse queue.
 * Placeholder for true SHM mapping — queues are in-process for now.
 */
class ShmPredictionClient : public PredictionClient {
public:
  ShmPredictionClient(PredictionContract c, std::size_t capacity = 1024,
                      LatencyTracker* t = nullptr)
      : PredictionClient(std::move(c), t)
      , req_queue_(capacity)
      , resp_queue_(capacity) {}

  Result<void> submit(const PredictionRequest& req) override {
    const auto t0 = steadyNs();
    if (!req_queue_.try_push(req)) {
      return Error{ErrorCode::QueueFull, "prediction request queue full"};
    }
    ++submitted_;
    if (tracker_) {
      tracker_->record(LatencyTracker::Stage::Ipc, (steadyNs() - t0) / 1000);
    }
    return Result<void>::success();
  }

  bool pushResponse(PredictionResponse resp) {
    if (handler_) handler_(resp);
    return resp_queue_.try_push(std::move(resp));
  }

  std::optional<PredictionRequest> popRequest() { return req_queue_.try_pop(); }
  std::optional<PredictionResponse> popResponse() { return resp_queue_.try_pop(); }

private:
  SpscQueue<PredictionRequest> req_queue_;
  SpscQueue<PredictionResponse> resp_queue_;
};

} // namespace crashcore
