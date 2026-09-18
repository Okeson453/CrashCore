#pragma once
#include "timing/timestamp.hpp"
#include "common/enums.hpp"
#include "common/types.hpp"
#include <string>
#include <unordered_map>
#include <mutex>
#include <optional>
#include <vector>

namespace crashcore {

enum class ComponentState : std::uint8_t {
  Unregistered = 0,
  Registered,
  Starting,
  Running,
  Stopping,
  Stopped,
  Failed
};

inline const char* toString(ComponentState s) noexcept {
  switch (s) {
    case ComponentState::Unregistered: return "unregistered";
    case ComponentState::Registered: return "registered";
    case ComponentState::Starting: return "starting";
    case ComponentState::Running: return "running";
    case ComponentState::Stopping: return "stopping";
    case ComponentState::Stopped: return "stopped";
    case ComponentState::Failed: return "failed";
  }
  return "unknown";
}

struct ComponentInfo {
  ComponentId id = ComponentId::Application;
  std::string name;
  ComponentState state = ComponentState::Unregistered;
  TimestampMs lastTransitionMs = 0;
  std::string lastError;
  int startOrder = 0;
};

class ComponentRegistry {
public:
  void registerComponent(ComponentId id, std::string name, int order = 0) {
    std::lock_guard lk(mu_);
    ComponentInfo info;
    info.id = id;
    info.name = std::move(name);
    info.state = ComponentState::Registered;
    info.startOrder = order;
    info.lastTransitionMs = nowMs();
    by_id_[id] = info;
  }

  bool setState(ComponentId id, ComponentState state, std::string error = {}) {
    std::lock_guard lk(mu_);
    auto it = by_id_.find(id);
    if (it == by_id_.end()) return false;
    it->second.state = state;
    it->second.lastTransitionMs = nowMs();
    if (!error.empty()) it->second.lastError = std::move(error);
    return true;
  }

  std::optional<ComponentInfo> get(ComponentId id) const {
    std::lock_guard lk(mu_);
    auto it = by_id_.find(id);
    if (it == by_id_.end()) return std::nullopt;
    return it->second;
  }

  std::vector<ComponentInfo> all() const {
    std::lock_guard lk(mu_);
    std::vector<ComponentInfo> out;
    out.reserve(by_id_.size());
    for (const auto& [_, v] : by_id_) out.push_back(v);
    return out;
  }

  bool allRunning() const {
    std::lock_guard lk(mu_);
    if (by_id_.empty()) return false;
    for (const auto& [_, v] : by_id_) {
      if (v.state != ComponentState::Running) return false;
    }
    return true;
  }

  std::size_t count() const {
    std::lock_guard lk(mu_);
    return by_id_.size();
  }

private:
  mutable std::mutex mu_;
  std::unordered_map<ComponentId, ComponentInfo> by_id_;
};

} // namespace crashcore
