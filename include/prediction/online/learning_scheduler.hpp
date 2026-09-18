#pragma once
#include "timing/timestamp.hpp"
#include <atomic>
#include <functional>
#include <mutex>
#include <string>
#include <vector>
namespace crashcore {
class LearningScheduler {
public:
  using Job = std::function<void()>;
  void schedule(const std::string& name, Job job, std::int64_t intervalMs) {
    std::lock_guard lk(mu_);
    jobs_.push_back({name, std::move(job), intervalMs, 0});
  }
  void tick() {
    std::lock_guard lk(mu_);
    auto now = nowMs();
    for (auto& j : jobs_) {
      if (now - j.lastRunMs >= j.intervalMs) {
        try { if (j.job) j.job(); ++runs_; } catch (...) { ++errors_; }
        j.lastRunMs = now;
      }
    }
  }
  std::uint64_t runCount() const noexcept { return runs_.load(); }
  std::uint64_t errorCount() const noexcept { return errors_.load(); }
private:
  struct Entry { std::string name; Job job; std::int64_t intervalMs; TimestampMs lastRunMs; };
  mutable std::mutex mu_;
  std::vector<Entry> jobs_;
  std::atomic<std::uint64_t> runs_{0}, errors_{0};
};
} // namespace crashcore
