#pragma once
#include "concurrency/task.hpp"
#include "concurrency/bounded_queue.hpp"
#include "common/types.hpp"
#include "timing/timestamp.hpp"
#include <atomic>
#include <string>
#include <thread>
#include <functional>

namespace crashcore {

/**
 * Single worker thread draining a BoundedQueue<Task>.
 * Uses std::jthread + stop_token for deterministic shutdown.
 * Informed by TestingEngine worker lifecycle concepts.
 */
class Worker {
public:
  using ErrorHandler = std::function<void(const std::string& name, const std::string& err)>;

  Worker(std::string name, BoundedQueue<Task>& queue)
      : name_(std::move(name)), queue_(queue) {}

  ~Worker() { stop(); }

  Worker(const Worker&) = delete;
  Worker& operator=(const Worker&) = delete;

  void start() {
    if (running_.exchange(true)) return;
    last_tick_ms_.store(nowMs(), std::memory_order_relaxed);
    thread_ = std::jthread([this](std::stop_token st) { run(st); });
  }

  void stop() {
    running_.store(false, std::memory_order_release);
    if (thread_.joinable()) {
      thread_.request_stop();
      thread_.join();
    }
  }

  void setErrorHandler(ErrorHandler h) { on_error_ = std::move(h); }

  const std::string& name() const noexcept { return name_; }
  bool running() const noexcept { return running_.load(std::memory_order_acquire); }

  WorkerHealth health() const {
    WorkerHealth h;
    h.name = name_;
    h.running = running();
    h.lastTickMs = last_tick_ms_.load(std::memory_order_relaxed);
    h.tasksProcessed = tasks_.load(std::memory_order_relaxed);
    h.lastError = last_error_;
    return h;
  }

  std::uint64_t tasksProcessed() const noexcept {
    return tasks_.load(std::memory_order_relaxed);
  }

private:
  void run(std::stop_token st) {
    while (!st.stop_requested() && running_.load(std::memory_order_acquire)) {
      auto t = queue_.pop(std::chrono::milliseconds(50));
      if (!t) continue;
      last_tick_ms_.store(nowMs(), std::memory_order_relaxed);
      try {
        (*t)();
        tasks_.fetch_add(1, std::memory_order_relaxed);
      } catch (const std::exception& ex) {
        last_error_ = ex.what();
        if (on_error_) on_error_(name_, last_error_);
      } catch (...) {
        last_error_ = "unknown exception";
        if (on_error_) on_error_(name_, last_error_);
      }
    }
    running_.store(false, std::memory_order_release);
  }

  std::string name_;
  BoundedQueue<Task>& queue_;
  std::atomic<bool> running_{false};
  std::atomic<TimestampMs> last_tick_ms_{0};
  std::atomic<std::uint64_t> tasks_{0};
  std::jthread thread_;
  ErrorHandler on_error_;
  std::string last_error_;
};

} // namespace crashcore
