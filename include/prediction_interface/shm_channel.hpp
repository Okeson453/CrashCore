#pragma once
/**
 * Shared-memory channel sketch for same-host IPC with Prediction Engine.
 * Production deployment maps a POSIX SHM region; this provides the ring layout
 * and serialization contract without requiring SHM at unit-test time.
 */
#include "prediction_interface/prediction_request.hpp"
#include "prediction_interface/prediction_response.hpp"
#include "common/types.hpp"
#include <atomic>
#include <cstdint>
#include <cstring>
#include <string>
#include <array>

namespace crashcore {

#pragma pack(push, 1)
struct ShmRequestSlot {
  std::atomic<std::uint32_t> sequence{0};
  std::uint64_t request_ns = 0;
  double current_mult = 0;
  char target_round[64]{};
  char correlation[64]{};
  char game_id[64]{};
  std::uint32_t flags = 0;
};

struct ShmResponseSlot {
  std::atomic<std::uint32_t> sequence{0};
  std::uint64_t response_ns = 0;
  double confidence = 0;
  double target_mult = 0;
  std::uint8_t decision = 0; // PredictionDecision
  char prediction_id[64]{};
  char target_round[64]{};
  char correlation[64]{};
  char model_version[32]{};
  std::uint32_t flags = 0;
};
#pragma pack(pop)

inline constexpr std::size_t SHM_QUEUE_SLOTS = 256;

struct ShmChannelLayout {
  std::atomic<std::uint64_t> magic{0x4352415348434F52ULL}; // "CRASHCOR"
  std::atomic<std::uint32_t> version{1};
  std::atomic<std::uint32_t> req_head{0};
  std::atomic<std::uint32_t> req_tail{0};
  std::atomic<std::uint32_t> resp_head{0};
  std::atomic<std::uint32_t> resp_tail{0};
  std::array<ShmRequestSlot, SHM_QUEUE_SLOTS> requests{};
  std::array<ShmResponseSlot, SHM_QUEUE_SLOTS> responses{};
};

inline void fillRequestSlot(ShmRequestSlot& slot, const PredictionRequest& req) {
  slot.request_ns = static_cast<std::uint64_t>(req.requestNs);
  slot.current_mult = req.currentMult;
  std::memset(slot.target_round, 0, sizeof(slot.target_round));
  std::memset(slot.correlation, 0, sizeof(slot.correlation));
  std::memset(slot.game_id, 0, sizeof(slot.game_id));
  std::strncpy(slot.target_round, req.targetRoundId.c_str(), sizeof(slot.target_round) - 1);
  std::strncpy(slot.correlation, req.correlationId.c_str(), sizeof(slot.correlation) - 1);
  std::strncpy(slot.game_id, req.gameId.c_str(), sizeof(slot.game_id) - 1);
}

inline PredictionResponse readResponseSlot(const ShmResponseSlot& slot) {
  PredictionResponse r;
  r.predictionId = slot.prediction_id;
  r.targetRoundId = slot.target_round;
  r.correlationId = slot.correlation;
  r.modelVersion = slot.model_version;
  r.confidence = slot.confidence;
  r.targetMult = slot.target_mult;
  r.decision = static_cast<PredictionDecision>(slot.decision);
  r.responseNs = static_cast<NanoTime>(slot.response_ns);
  r.valid = true;
  return r;
}

} // namespace crashcore
