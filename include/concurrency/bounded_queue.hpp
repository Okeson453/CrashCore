#pragma once
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <optional>
#include <vector>
#include <cstddef>

namespace crashcore {

/**
 * Bounded MPMC queue with backpressure.
 * Used for worker task distribution and non-hot-path channels.
 * Hot path prefers lock-free SPSC/MPSC.
 */
template <typename T>
class BoundedQueue {
public:
  explicit BoundedQueue(std::size_t capacity)
      : cap_(capacity < 1 ? 1 : capacity), buf_(cap_) {}

  bool try_push(T item) {
    std::unique_lock lk(mu_);
    if (closed_ || size_ >= cap_) return false;
    buf_[tail_] = std::move(item);
    tail_ = (tail_ + 1) % cap_;
    ++size_;
    ++pushes_;
    lk.unlock();
    cv_pop_.notify_one();
    return true;
  }

  bool push(T item, std::chrono::milliseconds timeout = std::chrono::milliseconds(0)) {
    std::unique_lock lk(mu_);
    if (timeout.count() <= 0) {
      cv_push_.wait(lk, [&] { return closed_ || size_ < cap_; });
    } else {
      if (!cv_push_.wait_for(lk, timeout, [&] { return closed_ || size_ < cap_; }))
        return false;
    }
    if (closed_) return false;
    buf_[tail_] = std::move(item);
    tail_ = (tail_ + 1) % cap_;
    ++size_;
    ++pushes_;
    lk.unlock();
    cv_pop_.notify_one();
    return true;
  }

  std::optional<T> try_pop() {
    std::unique_lock lk(mu_);
    if (size_ == 0) return std::nullopt;
    T item = std::move(buf_[head_]);
    head_ = (head_ + 1) % cap_;
    --size_;
    ++pops_;
    lk.unlock();
    cv_push_.notify_one();
    return item;
  }

  std::optional<T> pop(std::chrono::milliseconds timeout = std::chrono::milliseconds(50)) {
    std::unique_lock lk(mu_);
    if (!cv_pop_.wait_for(lk, timeout, [&] { return closed_ || size_ > 0; }))
      return std::nullopt;
    if (size_ == 0) return std::nullopt;
    T item = std::move(buf_[head_]);
    head_ = (head_ + 1) % cap_;
    --size_;
    ++pops_;
    lk.unlock();
    cv_push_.notify_one();
    return item;
  }

  void close() {
    {
      std::lock_guard lk(mu_);
      closed_ = true;
    }
    cv_push_.notify_all();
    cv_pop_.notify_all();
  }

  bool closed() const {
    std::lock_guard lk(mu_);
    return closed_;
  }

  std::size_t size() const {
    std::lock_guard lk(mu_);
    return size_;
  }

  std::size_t capacity() const noexcept { return cap_; }
  std::uint64_t push_count() const { std::lock_guard lk(mu_); return pushes_; }
  std::uint64_t pop_count() const { std::lock_guard lk(mu_); return pops_; }

private:
  std::size_t cap_;
  std::vector<T> buf_;
  std::size_t head_ = 0, tail_ = 0, size_ = 0;
  bool closed_ = false;
  mutable std::mutex mu_;
  std::condition_variable cv_push_, cv_pop_;
  std::uint64_t pushes_ = 0, pops_ = 0;
};

} // namespace crashcore
