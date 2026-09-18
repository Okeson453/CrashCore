#pragma once
/**
 * PORT_CONTRACT from TestingEngine src/lib/realtime/types.ts
 */
#include "common/types.hpp"
#include <string>
#include <optional>
#include <cstdint>

namespace crashcore::realtime {

enum class SourceKind : std::uint8_t { Socket = 0, Poll };
enum class RoundPhase : std::uint8_t { Prepare = 0, Begin, Progress, End };

enum class ConnectionStatus : std::uint8_t {
  Stopped = 0, Connecting, Connected, Degraded, Reconnecting, WafBlocked
};

struct RawSourceEvent {
  std::string sourceId;
  SourceKind sourceKind = SourceKind::Socket;
  std::string event;
  std::optional<std::string> gameId;
  std::optional<double> multiplier;
  std::optional<std::string> hash;
  std::optional<std::string> salt;
  std::optional<std::int64_t> beganAt;
  std::optional<std::int64_t> crashedAt;
  std::optional<std::int64_t> elapsedMs;
  TimestampMs receivedAt = 0;
  bool backfill = false;
};

struct NormalizedRoundEvent {
  std::string sourceId;
  SourceKind sourceKind = SourceKind::Socket;
  RoundPhase phase = RoundPhase::Prepare;
  std::string gameId;
  std::optional<double> multiplier;
  std::optional<std::string> hash;
  std::optional<std::string> salt;
  std::optional<std::int64_t> beganAt;
  std::optional<std::int64_t> crashedAt;
  TimestampMs receivedAt = 0;
  std::string rawEvent;
  bool backfill = false;
  std::optional<std::int64_t> elapsedMs;
};

struct ValidatedRoundEvent : NormalizedRoundEvent {
  TimestampMs acceptedAt = 0;
};

struct AdapterHealth {
  std::string sourceId;
  ConnectionStatus status = ConnectionStatus::Stopped;
  std::string transport;
  std::string lastError;
  TimestampMs lastEventAt = 0;
  std::string lastEventKind;
  std::uint64_t reconnectAttempts = 0;
  std::uint64_t totalReconnects = 0;
  std::string socketId;
};

struct MetricsSnapshot {
  std::uint64_t eventsReceived = 0;
  std::uint64_t eventsAccepted = 0;
  std::uint64_t duplicates = 0;
  std::uint64_t stale = 0;
  std::uint64_t invalid = 0;
  std::uint64_t missed = 0;
  std::uint64_t reconnects = 0;
  std::optional<double> lastArrivalLagMs;
  std::optional<double> lastProcessingMs;
  std::optional<double> lastPredictionMs;
  std::optional<double> lastDeliveryMs;
  std::optional<double> avgProcessingMs;
  std::optional<double> avgPredictionMs;
  std::optional<double> avgDeliveryMs;
  std::optional<double> lastE2eMs;
};

inline const char* toString(ConnectionStatus s) noexcept {
  switch (s) {
    case ConnectionStatus::Stopped: return "stopped";
    case ConnectionStatus::Connecting: return "connecting";
    case ConnectionStatus::Connected: return "connected";
    case ConnectionStatus::Degraded: return "degraded";
    case ConnectionStatus::Reconnecting: return "reconnecting";
    case ConnectionStatus::WafBlocked: return "waf_blocked";
  }
  return "stopped";
}

inline const char* toString(RoundPhase p) noexcept {
  switch (p) {
    case RoundPhase::Prepare: return "prepare";
    case RoundPhase::Begin: return "begin";
    case RoundPhase::Progress: return "progress";
    case RoundPhase::End: return "end";
  }
  return "prepare";
}

} // namespace crashcore::realtime
