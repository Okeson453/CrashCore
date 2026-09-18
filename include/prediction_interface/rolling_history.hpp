#pragma once
#include "common/types.hpp"
#include "timing/timestamp.hpp"
#include <deque>
#include <mutex>
#include <optional>
#include <vector>

namespace crashcore {

struct HistoryRound {
  GameId gameId;
  double multiplier = 0;
  TimestampMs crashedAtMs = 0;
  std::string hash;
};

class RollingHistoryBuffer {
public:
  explicit RollingHistoryBuffer(std::size_t capacity = 200) : capacity_(capacity) {}

  void push(HistoryRound r) {
    std::lock_guard lk(mu_);
    buf_.push_back(std::move(r));
    while (buf_.size() > capacity_) buf_.pop_front();
    ++pushes_;
  }

  void pushFromEvent(const CrashEvent& e) {
    if (e.kind != EventKind::End || !e.valid) return;
    HistoryRound r;
    r.gameId = e.gameId.empty() ? e.roundId : e.gameId;
    r.multiplier = e.crashPoint;
    r.crashedAtMs = e.endedAtMs ? e.endedAtMs : nowMs();
    r.hash = e.hash;
    push(std::move(r));
  }

  std::vector<HistoryRound> last(std::size_t n) const {
    std::lock_guard lk(mu_);
    std::vector<HistoryRound> out;
    if (buf_.empty()) return out;
    const auto start = buf_.size() > n ? buf_.size() - n : 0;
    for (std::size_t i = start; i < buf_.size(); ++i) out.push_back(buf_[i]);
    return out;
  }

  std::optional<double> lastMultiplier() const {
    std::lock_guard lk(mu_);
    if (buf_.empty()) return std::nullopt;
    return buf_.back().multiplier;
  }

  std::optional<double> average(std::size_t n) const {
    auto v = last(n);
    if (v.empty()) return std::nullopt;
    double s = 0;
    for (const auto& r : v) s += r.multiplier;
    return s / static_cast<double>(v.size());
  }

  std::size_t size() const { std::lock_guard lk(mu_); return buf_.size(); }
  std::uint64_t pushCount() const noexcept { return pushes_; }
  void clear() { std::lock_guard lk(mu_); buf_.clear(); }

private:
  std::size_t capacity_;
  mutable std::mutex mu_;
  std::deque<HistoryRound> buf_;
  std::uint64_t pushes_ = 0;
};

} // namespace crashcore
