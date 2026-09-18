#pragma once
/**
 * PORT_CONTRACT from TestingEngine live/decision-audit.ts
 * Records prediction interface decisions for forensics — no model logic.
 */
#include "common/types.hpp"
#include "prediction_interface/prediction_request.hpp"
#include "prediction_interface/prediction_response.hpp"
#include "timing/timestamp.hpp"
#include <deque>
#include <mutex>
#include <string>
#include <vector>

namespace crashcore {

struct DecisionAuditEntry {
  TimestampMs atMs = 0;
  std::string correlationId;
  std::string targetRoundId;
  PredictionDecision decision = PredictionDecision::Skip;
  double confidence = 0;
  double targetMult = 0;
  double currentMult = 0;
  std::int64_t latencyUs = 0;
  std::string modelVersion;
  bool timedOut = false;
  bool fenced = false;
};

class DecisionAuditLog {
public:
  explicit DecisionAuditLog(std::size_t maxEntries = 1000) : max_(maxEntries) {}

  void record(DecisionAuditEntry e) {
    if (e.atMs == 0) e.atMs = nowMs();
    std::lock_guard lk(mu_);
    entries_.push_back(std::move(e));
    while (entries_.size() > max_) entries_.pop_front();
    ++total_;
  }

  void recordResponse(const PredictionRequest& req, const PredictionResponse& resp,
                      std::int64_t latencyUs = 0) {
    DecisionAuditEntry e;
    e.correlationId = resp.correlationId.empty() ? req.correlationId : resp.correlationId;
    e.targetRoundId = resp.targetRoundId.empty() ? req.targetRoundId : resp.targetRoundId;
    e.decision = resp.decision;
    e.confidence = resp.confidence;
    e.targetMult = resp.targetMult;
    e.currentMult = req.currentMult;
    e.latencyUs = latencyUs;
    e.modelVersion = resp.modelVersion;
    record(std::move(e));
  }

  std::vector<DecisionAuditEntry> recent(std::size_t n = 50) const {
    std::lock_guard lk(mu_);
    std::vector<DecisionAuditEntry> out;
    const auto start = entries_.size() > n ? entries_.size() - n : 0;
    for (std::size_t i = start; i < entries_.size(); ++i) out.push_back(entries_[i]);
    return out;
  }

  std::uint64_t total() const noexcept { return total_; }
  std::size_t size() const { std::lock_guard lk(mu_); return entries_.size(); }

private:
  std::size_t max_;
  mutable std::mutex mu_;
  std::deque<DecisionAuditEntry> entries_;
  std::uint64_t total_ = 0;
};

} // namespace crashcore
