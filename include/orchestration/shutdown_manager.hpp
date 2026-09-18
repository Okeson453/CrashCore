#pragma once
#include "orchestration/component_registry.hpp"
#include "logging/structured_logger.hpp"
#include <functional>
#include <vector>
#include <algorithm>

namespace crashcore {

class ShutdownManager {
public:
  using StopHook = std::function<void(ComponentId)>;

  explicit ShutdownManager(ComponentRegistry& registry)
      : registry_(registry), log_(ComponentId::Orchestrator) {}

  void setHook(StopHook h) { hook_ = std::move(h); }

  void stopInReverseOrder() {
    auto comps = registry_.all();
    std::sort(comps.begin(), comps.end(),
              [](const ComponentInfo& a, const ComponentInfo& b) {
                return a.startOrder > b.startOrder;
              });
    for (const auto& c : comps) {
      if (c.state != ComponentState::Running && c.state != ComponentState::Failed) continue;
      registry_.setState(c.id, ComponentState::Stopping);
      log_.info("shutdown ", c.name);
      if (hook_) {
        try { hook_(c.id); } catch (...) {
          log_.warn("shutdown exception ", c.name);
        }
      }
      registry_.setState(c.id, ComponentState::Stopped);
    }
  }

private:
  ComponentRegistry& registry_;
  StopHook hook_;
  StructuredLogger log_;
};

} // namespace crashcore
