#pragma once
/**
 * Additional decode helpers bridging CrashEvent and realtime NormalizedRoundEvent.
 */
#include "ingestion/crash_event.hpp"
#include "realtime/normalizer.hpp"
#include "realtime/realtime_types.hpp"
#include "ingestion/socketio/socketio_protocol.hpp"
#include "timing/timestamp.hpp"
#include <optional>

namespace crashcore {

inline CrashEvent crashEventFromNormalized(const realtime::NormalizedRoundEvent& n) {
  CrashEvent e;
  e.valid = true;
  e.gameId = n.gameId;
  e.roundId = n.gameId;
  e.rawEventName = n.rawEvent;
  e.eventTimeMs = n.receivedAt;
  e.receivedAtNs = steadyNs();
  e.decodedAtNs = e.receivedAtNs;
  e.source = EventSource::NativeSocket;
  if (n.hash) e.hash = *n.hash;
  if (n.beganAt) e.beganAtMs = *n.beganAt;
  if (n.crashedAt) e.endedAtMs = *n.crashedAt;
  if (n.elapsedMs) e.elapsedMs = *n.elapsedMs;
  switch (n.phase) {
    case realtime::RoundPhase::Prepare:
      e.kind = EventKind::Prepare; break;
    case realtime::RoundPhase::Begin:
      e.kind = EventKind::Start; break;
    case realtime::RoundPhase::Progress:
      e.kind = EventKind::Progress;
      e.currentMult = n.multiplier.value_or(0);
      break;
    case realtime::RoundPhase::End:
      e.kind = EventKind::End;
      e.crashPoint = n.multiplier.value_or(0);
      break;
  }
  return e;
}

inline std::optional<CrashEvent> decodeAndNormalizeBinary(EventDecoder& dec,
                                                          const std::uint8_t* data, std::size_t len) {
  auto r = dec.decodeBinary(data, len);
  if (!r) return std::nullopt;
  auto raw = realtime::fromCrashEvent(r.value());
  auto n = realtime::normalizeSourceEvent(raw);
  if (!n) return r.value(); // return decoded even if normalize rejects
  return crashEventFromNormalized(*n);
}

} // namespace crashcore
