#pragma once
/**
 * PORT_BEHAVIOR from TestingEngine src/lib/realtime/normalizer.ts
 * mapEventName + normalizeSourceEvent
 */
#include "realtime/realtime_types.hpp"
#include "timing/timestamp.hpp"
#include <algorithm>
#include <cctype>
#include <optional>
#include <string>

namespace crashcore::realtime {

inline std::optional<RoundPhase> mapEventName(std::string event) {
  // Matches TE normalizer.ts:45-55 — "st" → End, "bg" → Begin
  for (auto& c : event) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
  if (event == "pr" || event == "prepare" || event == "ready") return RoundPhase::Prepare;
  if (event == "pg") return RoundPhase::Progress;
  if (event == "bg" || event == "begin" || event == "start") return RoundPhase::Begin;
  if (event == "ed" || event == "st" || event == "end" || event == "crash" || event == "bust")
    return RoundPhase::End;
  return std::nullopt;
}

inline TimestampMs toEpochMs(std::int64_t value) {
  if (value <= 0) return 0;
  if (value > 0 && value < 1'000'000'000'000LL) return static_cast<TimestampMs>(value * 1000);
  return static_cast<TimestampMs>(value);
}

inline std::optional<NormalizedRoundEvent> normalizeSourceEvent(const RawSourceEvent& raw) {
  auto phase = mapEventName(raw.event);
  if (!phase) return std::nullopt;

  NormalizedRoundEvent n;
  n.sourceId = raw.sourceId;
  n.sourceKind = raw.sourceKind;
  n.phase = *phase;
  n.rawEvent = raw.event;
  n.receivedAt = raw.receivedAt ? raw.receivedAt : nowMs();
  n.backfill = raw.backfill;
  n.elapsedMs = raw.elapsedMs;

  if (raw.gameId && !raw.gameId->empty()) n.gameId = *raw.gameId;
  if (raw.multiplier) n.multiplier = *raw.multiplier;
  if (raw.hash) n.hash = *raw.hash;
  if (raw.salt) n.salt = *raw.salt;
  if (raw.beganAt) n.beganAt = toEpochMs(*raw.beganAt);
  if (raw.crashedAt) n.crashedAt = toEpochMs(*raw.crashedAt);

  // Progress with elapsed but no multiplier
  if (n.phase == RoundPhase::Progress && !n.multiplier && n.elapsedMs) {
    n.multiplier = multiplierFromElapsed(*n.elapsedMs);
  }

  // End requires gameId for identity; progress may be anonymous mid-round
  if (n.phase != RoundPhase::Progress && n.gameId.empty()) {
    return std::nullopt;
  }
  return n;
}

inline RawSourceEvent fromCrashEvent(const CrashEvent& ev, std::string sourceId = "native-bc-socket") {
  // TE unwraps payload envelopes (data / payload / [0]) before field reads.
  // CrashEvent already carries unwrapped fields; map kinds to TE wire names.
  RawSourceEvent raw;
  raw.sourceId = std::move(sourceId);
  raw.sourceKind = SourceKind::Socket;
  raw.receivedAt = ev.eventTimeMs ? ev.eventTimeMs : static_cast<TimestampMs>(ev.receivedAtNs / 1'000'000);
  raw.gameId = ev.gameId.empty() ? std::nullopt : std::optional(ev.gameId);
  raw.hash = ev.hash.empty() ? std::nullopt : std::optional(ev.hash);
  if (ev.elapsedMs >= 0) raw.elapsedMs = ev.elapsedMs;
  switch (ev.kind) {
    case EventKind::Prepare:  raw.event = "pr"; break;
    case EventKind::Start:    raw.event = "bg"; raw.beganAt = ev.beganAtMs; break;
    case EventKind::Progress: raw.event = "pg"; raw.multiplier = ev.currentMult; break;
    case EventKind::End:
      raw.event = "ed";
      raw.multiplier = ev.crashPoint;
      raw.crashedAt = ev.endedAtMs;
      break;
    default: raw.event = ev.rawEventName.empty() ? "unknown" : ev.rawEventName; break;
  }
  return raw;
}

} // namespace crashcore::realtime
