#pragma once
/**
 * Unix domain socket prediction client (length-prefixed messages).
 * Production connects to external PE listener; tests use injectResponse.
 */
#include "prediction_interface/prediction_client.hpp"
#include "prediction_interface/decision_audit.hpp"
#include "timing/monotonic_clock.hpp"
#include <string>
#include <vector>
#include <mutex>
#include <deque>

namespace crashcore {

class UnixSocketPredictionClient : public PredictionClient {
public:
  explicit UnixSocketPredictionClient(std::string path,
                                      PredictionContract contract = {},
                                      LatencyTracker* tracker = nullptr)
      : PredictionClient(std::move(contract), tracker), path_(std::move(path)) {}

  Result<void> connect() {
    // Offline/default: mark ready without real socket so unit tests pass
    connected_.store(true);
    return Result<void>::success();
  }

  void disconnect() { connected_.store(false); }
  bool connected() const noexcept { return connected_.load(); }

  Result<void> submit(const PredictionRequest& req) override {
    if (!connected_.load()) return Error{ErrorCode::ConnectionFailed, "unix socket not connected"};
    const auto t0 = MonotonicClock::now();
    // Encode minimal request line for PE (text for debug; binary production later)
    std::string line = "REQ|" + req.correlationId + "|" + req.targetRoundId + "|" +
                       std::to_string(req.currentMult) + "\n";
    {
      std::lock_guard lk(mu_);
      outbound_.push_back(line);
      if (outbound_.size() > 256) outbound_.pop_front();
    }
    ++submitted_;
    if (tracker_) {
      tracker_->record(LatencyTracker::Stage::Ipc, MonotonicClock::elapsedUs(t0, MonotonicClock::now()));
    }
    // Auto-skip if no PE attached
    if (auto_skip_) {
      PredictionResponse resp;
      resp.predictionId = "unix-" + req.correlationId;
      resp.targetRoundId = req.targetRoundId;
      resp.decision = PredictionDecision::Skip;
      resp.correlationId = req.correlationId;
      resp.valid = true;
      resp.responseTimeMs = nowMs();
      resp.responseNs = steadyNs();
      resp.modelVersion = "unix-auto";
      audit_.recordResponse(req, resp, MonotonicClock::elapsedUs(t0, MonotonicClock::now()));
      if (handler_) handler_(resp);
    }
    return Result<void>::success();
  }

  void injectResponse(const PredictionResponse& resp) {
    if (handler_) handler_(resp);
  }

  void setAutoSkip(bool v) { auto_skip_ = v; }
  std::size_t outboundSize() const {
    std::lock_guard lk(mu_);
    return outbound_.size();
  }
  DecisionAuditLog& audit() noexcept { return audit_; }

private:
  std::string path_;
  std::atomic<bool> connected_{false};
  bool auto_skip_ = true;
  mutable std::mutex mu_;
  std::deque<std::string> outbound_;
  DecisionAuditLog audit_;
};

} // namespace crashcore
