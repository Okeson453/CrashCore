#pragma once
#include "orchestration/component_registry.hpp"
#include "common/result.hpp"
#include "logging/structured_logger.hpp"
#include <functional>
#include <vector>
#include <algorithm>

namespace crashcore {

class StartupManager {
public:
  using StartHook = std::function<Result<void>(ComponentId)>;

  explicit StartupManager(ComponentRegistry& registry)
      : registry_(registry), log_(ComponentId::Orchestrator) {}

  void setHook(StartHook h) { hook_ = std::move(h); }

  Result<void> startInOrder() {
    auto comps = registry_.all();
    std::sort(comps.begin(), comps.end(),
              [](const ComponentInfo& a, const ComponentInfo& b) {
                return a.startOrder < b.startOrder;
              });
    for (const auto& c : comps) {
      registry_.setState(c.id, ComponentState::Starting);
      log_.info("startup ", c.name);
      if (hook_) {
        auto r = hook_(c.id);
        if (!r) {
          registry_.setState(c.id, ComponentState::Failed, r.error().message);
          log_.error("startup failed ", c.name, ": ", r.error().message);
          return r.error();
        }
      }
      registry_.setState(c.id, ComponentState::Running);
    }
    return Result<void>::success();
  }

private:
  ComponentRegistry& registry_;
  StartHook hook_;
  StructuredLogger log_;
};

} // namespace crashcore
