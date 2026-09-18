#pragma once
/**
 * PORT_BEHAVIOR from TestingEngine src/lib/crash/socket-diagnostics.ts
 */
#include "common/types.hpp"
#include "ingestion/native_bc_socket.hpp"
#include "timing/timestamp.hpp"
#include <atomic>
#include <mutex>
#include <string>
#include <vector>
#include <deque>

namespace crashcore {

struct DiagnosticEvent {
  TimestampMs atMs = 0;
  std::string kind;
  std::string detail;
  std::uint64_t framesRx = 0;
  std::uint64_t reconnects = 0;
};

class SocketDiagnostics {
public:
  explicit SocketDiagnostics(std::size_t maxEvents = 200) : max_(maxEvents) {}

  void note(std::string kind, std::string detail = {},
            std::uint64_t framesRx = 0, std::uint64_t reconnects = 0) {
    DiagnosticEvent e;
    e.atMs = nowMs();
    e.kind = std::move(kind);
    e.detail = std::move(detail);
    e.framesRx = framesRx;
    e.reconnects = reconnects;
    std::lock_guard lk(mu_);
    events_.push_back(std::move(e));
    while (events_.size() > max_) events_.pop_front();
    ++notes_;
  }

  void noteStatus(NativeSocketStatus st, std::string detail = {}) {
    note(toString(st), std::move(detail));
  }

  void noteFrame(std::size_t bytes) {
    bytes_rx_ += bytes;
    ++frames_;
  }

  void noteError(std::string err) {
    last_error_ = err;
    note("error", std::move(err));
    ++errors_;
  }

  std::vector<DiagnosticEvent> recent(std::size_t n = 20) const {
    std::lock_guard lk(mu_);
    std::vector<DiagnosticEvent> out;
    const auto start = events_.size() > n ? events_.size() - n : 0;
    for (std::size_t i = start; i < events_.size(); ++i) out.push_back(events_[i]);
    return out;
  }

  std::string summary() const {
    return "frames=" + std::to_string(frames_.load()) +
           " bytes=" + std::to_string(bytes_rx_.load()) +
           " errors=" + std::to_string(errors_.load()) +
           " notes=" + std::to_string(notes_.load()) +
           " last_error=" + last_error_;
  }

  std::uint64_t frameCount() const noexcept { return frames_.load(); }
  std::uint64_t errorCount() const noexcept { return errors_.load(); }

private:
  std::size_t max_;
  mutable std::mutex mu_;
  std::deque<DiagnosticEvent> events_;
  std::atomic<std::uint64_t> frames_{0}, bytes_rx_{0}, errors_{0}, notes_{0};
  std::string last_error_;
};

} // namespace crashcore
