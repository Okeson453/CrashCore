#pragma once
#include "common/types.hpp"
#include "concurrency/bounded_queue.hpp"
#include <atomic>

namespace crashcore {

class OutboxQueue {
public:
  explicit OutboxQueue(std::size_t capacity = 2048) : q_(capacity) {}

  bool enqueue(OutboxItem item) {
    item.id = next_id_.fetch_add(1, std::memory_order_relaxed);
    if (!q_.try_push(std::move(item))) {
      ++drops_;
      return false;
    }
    ++enqueued_;
    return true;
  }

  std::optional<OutboxItem> dequeue(std::chrono::milliseconds timeout = std::chrono::milliseconds(100)) {
    auto item = q_.pop(timeout);
    if (item) ++dequeued_;
    return item;
  }

  std::size_t size() const { return q_.size(); }
  std::uint64_t enqueuedCount() const { return enqueued_.load(std::memory_order_relaxed); }
  std::uint64_t dequeuedCount() const { return dequeued_.load(std::memory_order_relaxed); }
  std::uint64_t dropCount() const { return drops_.load(std::memory_order_relaxed); }

private:
  BoundedQueue<OutboxItem> q_;
  std::atomic<std::uint64_t> next_id_{1};
  std::atomic<std::uint64_t> enqueued_{0}, dequeued_{0}, drops_{0};
};

} // namespace crashcore
