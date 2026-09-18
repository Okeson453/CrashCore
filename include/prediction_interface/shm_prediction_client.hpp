#pragma once
/**
 * Shared-memory style prediction client using an in-process lock-free-ish ring.
 * Mirrors the ShmChannelLayout contract; production maps POSIX SHM region.
 * Never runs models — only IPC to external PE.
 */
#include "prediction_interface/prediction_client.hpp"
#include "prediction_interface/shm_channel.hpp"
#include "prediction_interface/decision_audit.hpp"
#include "timing/monotonic_clock.hpp"
#include <array>
#include <atomic>
#include <cstring>
#include <thread>

namespace crashcore {

class ShmPredictionClient : public PredictionClient {
public:
  explicit ShmPredictionClient(PredictionContract contract = {},
                               LatencyTracker* tracker = nullptr)
      : PredictionClient(std::move(contract), tracker)
      , layout_() {
    layout_.magic.store(0x4352415348434F52ULL);
    layout_.version.store(1);
  }

  Result<void> submit(const PredictionRequest& req) override {
    const auto t0 = MonotonicClock::now();
    const auto head = layout_.req_head.load(std::memory_order_relaxed);
    const auto next = (head + 1) % SHM_QUEUE_SLOTS;
    if (next == layout_.req_tail.load(std::memory_order_acquire)) {
      return Error{ErrorCode::QueueFull, "shm request ring full"};
    }
    auto& slot = layout_.requests[head % SHM_QUEUE_SLOTS];
    fillRequestSlot(slot, req);
    slot.sequence.store(static_cast<std::uint32_t>(head + 1), std::memory_order_release);
    layout_.req_head.store(next, std::memory_order_release);
    ++submitted_;

    // In-process PE simulator path: auto-respond Skip unless handler injects
    // Real PE would write response slots from another process.
    if (auto_respond_) {
      PredictionResponse resp;
      resp.predictionId = "shm-" + req.correlationId;
      resp.targetRoundId = req.targetRoundId;
      resp.decision = PredictionDecision::Skip;
      resp.confidence = 0;
      resp.correlationId = req.correlationId;
      resp.modelVersion = "shm-auto";
      resp.responseTimeMs = nowMs();
      resp.responseNs = steadyNs();
      resp.valid = true;
      writeResponse(resp);
      if (handler_) handler_(resp);
      audit_.recordResponse(req, resp, MonotonicClock::elapsedUs(t0, MonotonicClock::now()));
    }
    if (tracker_) {
      tracker_->record(LatencyTracker::Stage::Ipc, MonotonicClock::elapsedUs(t0, MonotonicClock::now()));
    }
    return Result<void>::success();
  }

  void writeResponse(const PredictionResponse& resp) {
    const auto head = layout_.resp_head.load(std::memory_order_relaxed);
    const auto next = (head + 1) % SHM_QUEUE_SLOTS;
    auto& slot = layout_.responses[head % SHM_QUEUE_SLOTS];
    slot.response_ns = static_cast<std::uint64_t>(resp.responseNs);
    slot.confidence = resp.confidence;
    slot.target_mult = resp.targetMult;
    slot.decision = static_cast<std::uint8_t>(resp.decision);
    std::memset(slot.prediction_id, 0, sizeof(slot.prediction_id));
    std::memset(slot.target_round, 0, sizeof(slot.target_round));
    std::memset(slot.correlation, 0, sizeof(slot.correlation));
    std::memset(slot.model_version, 0, sizeof(slot.model_version));
    std::strncpy(slot.prediction_id, resp.predictionId.c_str(), sizeof(slot.prediction_id) - 1);
    std::strncpy(slot.target_round, resp.targetRoundId.c_str(), sizeof(slot.target_round) - 1);
    std::strncpy(slot.correlation, resp.correlationId.c_str(), sizeof(slot.correlation) - 1);
    std::strncpy(slot.model_version, resp.modelVersion.c_str(), sizeof(slot.model_version) - 1);
    slot.sequence.store(static_cast<std::uint32_t>(head + 1), std::memory_order_release);
    layout_.resp_head.store(next, std::memory_order_release);
  }

  std::optional<PredictionResponse> pollResponseSlot() {
    const auto tail = layout_.resp_tail.load(std::memory_order_relaxed);
    const auto head = layout_.resp_head.load(std::memory_order_acquire);
    if (tail == head) return std::nullopt;
    auto& slot = layout_.responses[tail % SHM_QUEUE_SLOTS];
    auto r = readResponseSlot(slot);
    layout_.resp_tail.store((tail + 1) % SHM_QUEUE_SLOTS, std::memory_order_release);
    return r;
  }

  void setAutoRespond(bool v) { auto_respond_ = v; }
  ShmChannelLayout& layout() noexcept { return layout_; }
  DecisionAuditLog& audit() noexcept { return audit_; }

private:
  ShmChannelLayout layout_;
  DecisionAuditLog audit_;
  bool auto_respond_ = true;
};

} // namespace crashcore
