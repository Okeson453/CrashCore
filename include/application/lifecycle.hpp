#pragma once
#include <atomic>
#include <csignal>

namespace crashcore {

class Lifecycle {
public:
  static Lifecycle& instance() {
    static Lifecycle inst;
    return inst;
  }

  void requestShutdown() noexcept { shutdown_.store(true, std::memory_order_release); }
  bool shutdownRequested() const noexcept { return shutdown_.load(std::memory_order_acquire); }

  static void installSignalHandlers() {
    std::signal(SIGINT, [](int) { Lifecycle::instance().requestShutdown(); });
    std::signal(SIGTERM, [](int) { Lifecycle::instance().requestShutdown(); });
  }

private:
  std::atomic<bool> shutdown_{false};
};

} // namespace crashcore
