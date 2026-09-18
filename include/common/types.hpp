#pragma once
/**
 * Core domain types for CrashCore.
 * Mapped from TestingEngine src/lib/crash/types.ts and realtime types.
 */
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>
#include <array>
#include <chrono>

namespace crashcore {

using RoundId       = std::string;
using PredictionId  = std::string;
using GameId        = std::string;
using TimestampMs   = std::int64_t;
using NanoTime      = std::int64_t;
using SequenceNum   = std::uint64_t;

struct CrashRound {
  GameId        gameId;
  double        multiplier   = 0.0;
  std::string   hash;
  std::string   salt;
  TimestampMs   beganAtMs    = 0;
  TimestampMs   crashedAtMs  = 0;
  SequenceNum   sequence     = 0;
  bool          hasHash      = false;
  bool          hasSalt      = false;
};

enum class EventKind : std::uint8_t {
  Unknown = 0,
  Progress,
  Start,
  End,
  Prepare,
  BetsOpen,
  BetsClosed,
  Heartbeat,
  Connect,
  Disconnect,
  Error
};

enum class EventSource : std::uint8_t {
  Unknown = 0,
  NativeSocket,
  SocketIo,
  RestPoll,
  Replay,
  Synthetic
};

struct CrashEvent {
  EventKind     kind          = EventKind::Unknown;
  EventSource   source        = EventSource::Unknown;
  RoundId       roundId;
  GameId        gameId;
  double        crashPoint    = 0.0;
  double        currentMult   = 0.0;
  TimestampMs   beganAtMs     = 0;
  TimestampMs   endedAtMs     = 0;
  TimestampMs   eventTimeMs   = 0;
  SequenceNum   sequence      = 0;
  NanoTime      receivedAtNs  = 0;
  NanoTime      decodedAtNs   = 0;
  NanoTime      routedAtNs    = 0;
  std::string   hash;
  std::string   rawEventName;
  std::int64_t  elapsedMs     = -1;
  bool          valid         = false;
};

enum class PredictionDecision : std::uint8_t {
  Skip = 0,
  Enter,
  Hold
};

enum class PredictionOutcome : std::uint8_t {
  Pending = 0,
  Win,
  Loss,
  Void,
  Stale,
  Duplicate
};

struct PredictionRequest {
  RoundId       targetRoundId;
  GameId        gameId;
  TimestampMs   requestTimeMs = 0;
  NanoTime      requestNs     = 0;
  SequenceNum   sequence      = 0;
  double        currentMult   = 0.0;
  std::string   correlationId;
};

struct PredictionResponse {
  PredictionId      predictionId;
  RoundId           targetRoundId;
  PredictionDecision decision     = PredictionDecision::Skip;
  double            confidence    = 0.0;
  double            probability   = 0.0;
  double            targetMult    = 0.0;
  TimestampMs       responseTimeMs = 0;
  NanoTime          responseNs    = 0;
  std::string       correlationId;
  std::string       modelVersion;
  bool              valid         = false;
};

struct PredictionEvent {
  PredictionId      predictionId;
  RoundId           targetRoundId;
  GameId            gameId;
  PredictionDecision decision     = PredictionDecision::Skip;
  double            confidence    = 0.0;
  double            probability   = 0.0;
  double            entryMult     = 0.0;
  double            targetMult    = 0.0;
  TimestampMs       createdAtMs   = 0;
  NanoTime          createdAtNs   = 0;
  std::string       correlationId;
  std::string       modelVersion;
  bool              delivered     = false;
};

struct Outcome {
  PredictionId      predictionId;
  RoundId           roundId;
  PredictionOutcome result        = PredictionOutcome::Pending;
  double            actualMult    = 0.0;
  double            targetMult    = 0.0;
  TimestampMs       resolvedAtMs  = 0;
  bool              isWin         = false;
};

enum class SignalKind : std::uint8_t {
  Prediction = 0,
  Outcome,
  Heartbeat,
  SystemAlert
};

struct Signal {
  SignalKind        kind          = SignalKind::Prediction;
  PredictionId      predictionId;
  RoundId           roundId;
  std::string       text;
  TimestampMs       createdAtMs   = 0;
  std::string       correlationId;
  bool              urgent        = false;
};

enum class OutboxState : std::uint8_t {
  Pending = 0,
  Claimed,
  InFlight,
  Delivered,
  Failed,
  DeadLetter
};

struct OutboxItem {
  std::uint64_t     id            = 0;
  OutboxState       state         = OutboxState::Pending;
  Signal            signal;
  TimestampMs       createdAtMs   = 0;
  TimestampMs       claimedAtMs   = 0;
  TimestampMs       deliveredAtMs = 0;
  int               attempts      = 0;
  std::string       lastError;
  std::string       dedupeKey;
};

struct ConnectionHealth {
  bool          connected     = false;
  TimestampMs   lastMessageMs = 0;
  TimestampMs   lastPongMs    = 0;
  std::uint64_t reconnects    = 0;
  std::string   lastError;
};

struct QueueHealth {
  std::size_t   size          = 0;
  std::size_t   capacity      = 0;
  std::uint64_t drops         = 0;
  std::uint64_t pushes        = 0;
  std::uint64_t pops          = 0;
};

struct WorkerHealth {
  std::string   name;
  bool          running       = false;
  TimestampMs   lastTickMs    = 0;
  std::uint64_t tasksProcessed = 0;
  std::string   lastError;
};

inline constexpr double HIGH_THRESHOLD  = 2.0;
inline constexpr double MOON_THRESHOLD  = 10.0;
inline constexpr std::string_view SOURCE_URL = "https://bc.game/game/crash";

inline bool isHigh(double m)  noexcept { return m >= HIGH_THRESHOLD; }
inline bool isMoon(double m)  noexcept { return m >= MOON_THRESHOLD; }

} // namespace crashcore
