#pragma once
#include <atomic>
#include <cstddef>
#include <optional>
#include <vector>

namespace crashcore {

/** Single-producer single-consumer ring buffer (power-of-two capacity). */
template <typename T>
class SpscQueue {
public:
  explicit SpscQueue(std::size_t capacity_pow2) {
    std::size_t c = 2;
    while (c < capacity_pow2) c <<= 1;
    cap_ = c;
    mask_ = c - 1;
    buf_.resize(c);
  }

  bool try_push(const T& item) {
    const auto t = tail_.load(std::memory_order_relaxed);
    const auto next = (t + 1) & mask_;
    if (next == head_.load(std::memory_order_acquire)) return false;
    buf_[t] = item;
    tail_.store(next, std::memory_order_release);
    return true;
  }

  bool try_push(T&& item) {
    const auto t = tail_.load(std::memory_order_relaxed);
    const auto next = (t + 1) & mask_;
    if (next == head_.load(std::memory_order_acquire)) return false;
    buf_[t] = std::move(item);
    tail_.store(next, std::memory_order_release);
    return true;
  }

  std::optional<T> try_pop() {
    const auto h = head_.load(std::memory_order_relaxed);
    if (h == tail_.load(std::memory_order_acquire)) return std::nullopt;
    T item = std::move(buf_[h]);
    head_.store((h + 1) & mask_, std::memory_order_release);
    return item;
  }

  std::size_t size_approx() const {
    const auto t = tail_.load(std::memory_order_acquire);
    const auto h = head_.load(std::memory_order_acquire);
    return (t - h) & mask_;
  }

  std::size_t capacity() const noexcept { return cap_; }

private:
  std::size_t cap_ = 0, mask_ = 0;
  std::vector<T> buf_;
  alignas(64) std::atomic<std::size_t> head_{0};
  alignas(64) std::atomic<std::size_t> tail_{0};
};

} // namespace crashcore
