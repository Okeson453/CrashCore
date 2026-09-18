#pragma once
#include "common/types.hpp"
#include "concurrency/bounded_queue.hpp"
#include "concurrency/task.hpp"
#include <atomic>
#include <memory>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <vector>
#include <chrono>

namespace crashcore {
class WorkerPool {
public:
  explicit WorkerPool(std::size_t num_workers = 4, std::size_t queue_capacity = 1024)
      : n_(num_workers ? num_workers : 1), capacity_(queue_capacity) {
    for (std::size_t i = 0; i < n_; ++i)
      queues_.push_back(std::make_unique<BoundedQueue<Task>>(capacity_));
  }
  ~WorkerPool() { stop(); }
  void start() {
    if (running_.exchange(true)) return;
    for (std::size_t i = 0; i < n_; ++i)
      workers_.emplace_back([this, i](std::stop_token st) { workerLoop(i, st); });
  }
  void stop() {
    running_.store(false);
    cv_.notify_all();
    for (auto& q : queues_) q->close();
    for (auto& w : workers_) { if (w.joinable()) { w.request_stop(); w.join(); } }
    workers_.clear();
  }
  bool try_submit(Task task) {
    std::size_t best = 0, bestSize = queues_[0]->size();
    for (std::size_t i = 1; i < n_; ++i) {
      auto s = queues_[i]->size();
      if (s < bestSize) { bestSize = s; best = i; }
    }
    if (queues_[best]->try_push(std::move(task))) { ++submits_; cv_.notify_one(); return true; }
    for (std::size_t i = 0; i < n_; ++i)
      if (queues_[i]->try_push(std::move(task))) { ++submits_; return true; }
    ++drops_; return false;
  }
  bool submit(Task task, std::chrono::milliseconds timeout = std::chrono::milliseconds(100)) {
    if (try_submit(std::move(task))) return true;
    if (queues_[0]->push(std::move(task), timeout)) { ++submits_; return true; }
    ++drops_; return false;
  }
  std::size_t worker_count() const noexcept { return n_; }
  std::uint64_t submit_count() const noexcept { return submits_.load(); }
  std::uint64_t drop_count() const noexcept { return drops_.load(); }
  std::uint64_t steal_count() const noexcept { return steals_.load(); }
  QueueHealth health() const { return queue_health(); }
  QueueHealth queue_health() const {
    QueueHealth h;
    for (auto& q : queues_) {
      h.size += q->size(); h.capacity += q->capacity();
      h.pushes += q->push_count(); h.pops += q->pop_count();
    }
    return h;
  }
  std::size_t queue_size() const {
    std::size_t s = 0; for (auto& q : queues_) s += q->size(); return s;
  }
private:
  void workerLoop(std::size_t id, std::stop_token st) {
    while (!st.stop_requested() && running_.load()) {
      auto item = queues_[id]->try_pop();
      if (!item) {
        for (std::size_t j = 1; j < n_; ++j) {
          item = queues_[(id + j) % n_]->try_pop();
          if (item) { ++steals_; break; }
        }
      }
      if (item) {
        try { (*item)(); } catch (...) {}
      } else {
        // Block until try_submit notifies or stop — no 100µs busy-spin
        std::unique_lock lk(cv_mu_);
        cv_.wait_for(lk, std::chrono::milliseconds(5), [&] {
          return !running_.load() || st.stop_requested() || queue_size() > 0;
        });
      }
    }
  }
  void notify() { cv_.notify_all(); }
  std::size_t n_, capacity_;
  std::vector<std::unique_ptr<BoundedQueue<Task>>> queues_;
  std::vector<std::jthread> workers_;
  std::atomic<bool> running_{false};
  std::atomic<std::uint64_t> submits_{0}, drops_{0}, steals_{0};
  std::mutex cv_mu_;
  std::condition_variable cv_;
};
} // namespace crashcore
