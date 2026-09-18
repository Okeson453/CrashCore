#pragma once
#include "common/types.hpp"
#include "common/enums.hpp"
#include "timing/clock.hpp"

namespace crashcore {

// CrashEvent is defined in common/types.hpp — this header re-exports for
// ingestion-local includes and adds a few helpers.

inline bool isTerminal(const CrashEvent& e) noexcept {
  return e.kind == EventKind::End;
}

inline bool isLiveProgress(const CrashEvent& e) noexcept {
  return e.kind == EventKind::Progress && e.valid;
}

inline bool hasIdentity(const CrashEvent& e) noexcept {
  return !e.gameId.empty() || !e.roundId.empty();
}

} // namespace crashcore
