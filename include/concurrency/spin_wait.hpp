#pragma once
/**
 * Lightweight spin/wait utilities for SPSC polling on hot path.
 */
#include <atomic>
#include <chrono>
#include <thread>
#include <cstdint>

namespace crashcore {

class SpinWait {
public:
  void reset() noexcept { count_ = 0; }

  void spin_once() noexcept {
    if (count_ < 16) {
      // pause
      for (int i = 0; i < (1 << count_); ++i) {
        #if defined(__x86_64__) || defined(_M_X64)
        __asm__ __volatile__("pause");
        #endif
      }
      ++count_;
    } else if (count_ < 32) {
      std::this_thread::yield();
      ++count_;
    } else {
      std::this_thread::sleep_for(std::chrono::microseconds(50));
    }
  }

private:
  int count_ = 0;
};

template <typename Pred>
bool spin_until(Pred pred, std::chrono::microseconds budget = std::chrono::microseconds(1000)) {
  SpinWait sw;
  const auto deadline = std::chrono::steady_clock::now() + budget;
  while (!pred()) {
    if (std::chrono::steady_clock::now() >= deadline) return false;
    sw.spin_once();
  }
  return true;
}

} // namespace crashcore
