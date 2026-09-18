#pragma once
#include <atomic>
#include <cstddef>
#include <optional>
#include <vector>
#include <memory>

namespace crashcore {

/**
 * Multi-producer single-consumer queue (lock-free for producers via CAS).
 * Suitable for multiple sources feeding one consumer worker.
 */
template <typename T>
class MpscQueue {
public:
  explicit MpscQueue(std::size_t capacity_pow2) {
    std::size_t c = 2;
    while (c < capacity_pow2) c <<= 1;
    cap_ = c;
    mask_ = c - 1;
    slots_ = std::make_unique<Slot[]>(c);
    for (std::size_t i = 0; i < c; ++i)
      slots_[i].sequence.store(i, std::memory_order_relaxed);
    head_.store(0, std::memory_order_relaxed);
    tail_.store(0, std::memory_order_relaxed);
  }

  bool try_push(T item) {
    std::size_t pos = tail_.load(std::memory_order_relaxed);
    for (;;) {
      Slot& slot = slots_[pos & mask_];
      const std::size_t seq = slot.sequence.load(std::memory_order_acquire);
      const std::ptrdiff_t dif = static_cast<std::ptrdiff_t>(seq) - static_cast<std::ptrdiff_t>(pos);
      if (dif == 0) {
        if (tail_.compare_exchange_weak(pos, pos + 1, std::memory_order_relaxed)) {
          slot.data = std::move(item);
          slot.sequence.store(pos + 1, std::memory_order_release);
          pushes_.fetch_add(1, std::memory_order_relaxed);
          return true;
        }
      } else if (dif < 0) {
        return false; // full
      } else {
        pos = tail_.load(std::memory_order_relaxed);
      }
    }
  }

  std::optional<T> try_pop() {
    const std::size_t pos = head_.load(std::memory_order_relaxed);
    Slot& slot = slots_[pos & mask_];
    const std::size_t seq = slot.sequence.load(std::memory_order_acquire);
    if (seq != pos + 1) return std::nullopt;
    T item = std::move(slot.data);
    slot.sequence.store(pos + mask_ + 1, std::memory_order_release);
    head_.store(pos + 1, std::memory_order_relaxed);
    pops_.fetch_add(1, std::memory_order_relaxed);
    return item;
  }

  std::size_t size_approx() const {
    const auto t = tail_.load(std::memory_order_acquire);
    const auto h = head_.load(std::memory_order_acquire);
    return t - h;
  }

  std::size_t capacity() const noexcept { return cap_; }
  std::uint64_t push_count() const { return pushes_.load(std::memory_order_relaxed); }
  std::uint64_t pop_count() const { return pops_.load(std::memory_order_relaxed); }

private:
  struct Slot {
    std::atomic<std::size_t> sequence;
    T data;
  };

  std::size_t cap_ = 0, mask_ = 0;
  std::unique_ptr<Slot[]> slots_;
  alignas(64) std::atomic<std::size_t> head_{0};
  alignas(64) std::atomic<std::size_t> tail_{0};
  std::atomic<std::uint64_t> pushes_{0};
  std::atomic<std::uint64_t> pops_{0};
};

} // namespace crashcore
