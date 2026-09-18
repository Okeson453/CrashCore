#pragma once
#include <algorithm>
/**
 * Component orchestrator — deterministic startup/shutdown order.
 * Informed by TestingEngine live/boot.ts and supervisor concepts.
 */
#include "orchestration/component_registry.hpp"
#include "orchestration/startup_manager.hpp"
#include "orchestration/shutdown_manager.hpp"
#include "common/result.hpp"
#include "common/enums.hpp"
#include "logging/structured_logger.hpp"
#include <functional>
#include <string>
#include <vector>
#include <atomic>

namespace crashcore {

class Orchestrator {
public:
  using StartFn = std::function<Result<void>()>;
  using StopFn = std::function<void()>;

  struct Component {
    ComponentId id;
    std::string name;
    StartFn start;
    StopFn stop;
    int order = 0; // lower starts first, stops last
  };

  Orchestrator() : log_(ComponentId::Orchestrator) {}

  void add(Component c) {
    components_.push_back(std::move(c));
  }

  Result<void> startAll() {
    auto sorted = components_;
    std::sort(sorted.begin(), sorted.end(),
              [](const Component& a, const Component& b) { return a.order < b.order; });
    for (auto& c : sorted) {
      log_.info("starting ", c.name);
      if (c.start) {
        auto r = c.start();
        if (!r) {
          log_.error("failed to start ", c.name, ": ", r.error().message);
          // stop already-started
          stopAll();
          return r.error();
        }
      }
      started_.push_back(c.id);
    }
    running_.store(true, std::memory_order_release);
    log_.info("all components started");
    return Result<void>::success();
  }

  void stopAll() {
    // reverse order
    for (auto it = started_.rbegin(); it != started_.rend(); ++it) {
      for (auto& c : components_) {
        if (c.id == *it && c.stop) {
          log_.info("stopping ", c.name);
          try { c.stop(); } catch (...) {}
        }
      }
    }
    started_.clear();
    running_.store(false, std::memory_order_release);
  }

  bool running() const noexcept { return running_.load(std::memory_order_acquire); }
  std::size_t componentCount() const noexcept { return components_.size(); }

private:
  std::vector<Component> components_;
  std::vector<ComponentId> started_;
  std::atomic<bool> running_{false};
  StructuredLogger log_;
};

} // namespace crashcore
