#pragma once
/**
 * Live round state machine.
 * Mapped from TestingEngine live/live-round-state.ts and realtime types.
 * Tracks prepare → begin → progress → end transitions per gameId.
 */
#include "common/types.hpp"
#include "ingestion/crash_event.hpp"
#include "timing/timestamp.hpp"
#include <optional>
#include <string>
#include <mutex>
#include <unordered_map>
#include <atomic>

namespace crashcore {

enum class RoundPhase : std::uint8_t {
  None = 0,
  Prepare,
  Begin,
  Progress,
  End
};

inline const char* toString(RoundPhase p) noexcept {
  switch (p) {
    case RoundPhase::None: return "none";
    case RoundPhase::Prepare: return "prepare";
    case RoundPhase::Begin: return "begin";
    case RoundPhase::Progress: return "progress";
    case RoundPhase::End: return "end";
  }
  return "unknown";
}

inline RoundPhase phaseFromEventKind(EventKind k) noexcept {
  switch (k) {
    case EventKind::Prepare:  return RoundPhase::Prepare;
    case EventKind::Start:    return RoundPhase::Begin;
    case EventKind::Progress: return RoundPhase::Progress;
    case EventKind::End:      return RoundPhase::End;
    default:                  return RoundPhase::None;
  }
}

struct LiveRoundState {
  GameId      gameId;
  RoundPhase  phase = RoundPhase::None;
  double      multiplier = 0.0;
  double      finalMult = 0.0;
  TimestampMs beganAtMs = 0;
  TimestampMs endedAtMs = 0;
  TimestampMs updatedAtMs = 0;
  std::string hash;
  SequenceNum sequence = 0;
  bool        sealed = false; // end received

  bool isLive() const noexcept {
    return phase == RoundPhase::Begin || phase == RoundPhase::Progress;
  }
};

class RoundStateRegistry {
public:
  /** Apply event; returns updated state if accepted. */
  std::optional<LiveRoundState> apply(const CrashEvent& ev) {
    if (!ev.valid) return std::nullopt;
    const auto id = !ev.gameId.empty() ? ev.gameId : ev.roundId;
    if (id.empty() && ev.kind != EventKind::Progress) return std::nullopt;

    std::lock_guard lk(mu_);
    // Progress without id attaches to current
    std::string key = id;
    if (key.empty() && !current_id_.empty()) key = current_id_;
    if (key.empty()) return std::nullopt;

    auto& st = by_id_[key];
    st.gameId = key;
    st.updatedAtMs = nowMs();
    st.sequence = ev.sequence;

    const auto newPhase = phaseFromEventKind(ev.kind);
    switch (ev.kind) {
      case EventKind::Prepare:
        st.phase = RoundPhase::Prepare;
        st.sealed = false;
        st.multiplier = 0;
        st.finalMult = 0;
        current_id_ = key;
        break;
      case EventKind::Start:
        st.phase = RoundPhase::Begin;
        st.beganAtMs = ev.beganAtMs ? ev.beganAtMs : nowMs();
        st.sealed = false;
        current_id_ = key;
        break;
      case EventKind::Progress:
        if (st.sealed) return std::nullopt; // ignore progress after end
        st.phase = RoundPhase::Progress;
        st.multiplier = ev.currentMult > 0 ? ev.currentMult : st.multiplier;
        break;
      case EventKind::End:
        st.phase = RoundPhase::End;
        st.finalMult = ev.crashPoint > 0 ? ev.crashPoint : ev.currentMult;
        st.multiplier = st.finalMult;
        st.endedAtMs = ev.endedAtMs ? ev.endedAtMs : nowMs();
        if (!ev.hash.empty()) st.hash = ev.hash;
        st.sealed = true;
        ++ended_;
        break;
      default:
        return std::nullopt;
    }
    ++applied_;
    return st;
  }

  std::optional<LiveRoundState> get(const GameId& id) const {
    std::lock_guard lk(mu_);
    auto it = by_id_.find(id);
    if (it == by_id_.end()) return std::nullopt;
    return it->second;
  }

  std::optional<LiveRoundState> current() const {
    std::lock_guard lk(mu_);
    if (current_id_.empty()) return std::nullopt;
    auto it = by_id_.find(current_id_);
    if (it == by_id_.end()) return std::nullopt;
    return it->second;
  }

  std::size_t size() const {
    std::lock_guard lk(mu_);
    return by_id_.size();
  }

  /** Drop sealed rounds older than maxAgeMs. */
  std::size_t prune(TimestampMs maxAgeMs) {
    const auto now = nowMs();
    std::lock_guard lk(mu_);
    std::size_t n = 0;
    for (auto it = by_id_.begin(); it != by_id_.end(); ) {
      if (it->second.sealed && it->second.endedAtMs > 0 &&
          now - it->second.endedAtMs > maxAgeMs) {
        if (it->first == current_id_) current_id_.clear();
        it = by_id_.erase(it);
        ++n;
      } else {
        ++it;
      }
    }
    return n;
  }

  std::uint64_t appliedCount() const noexcept {
    return applied_.load(std::memory_order_relaxed);
  }
  std::uint64_t endedCount() const noexcept {
    return ended_.load(std::memory_order_relaxed);
  }

private:
  mutable std::mutex mu_;
  std::unordered_map<GameId, LiveRoundState> by_id_;
  GameId current_id_;
  std::atomic<std::uint64_t> applied_{0};
  std::atomic<std::uint64_t> ended_{0};
};

} // namespace crashcore
