#pragma once
#include "timing/timestamp.hpp"
#include <string>
#include <unordered_map>
#include <cstdint>

namespace crashcore {

class LatencyTimer {
public:
  void start() { start_ns_ = steadyNs(); marks_.clear(); }
  void mark(const std::string& name) { marks_[name] = steadyNs(); }
  std::int64_t sinceStartUs() const { return (steadyNs() - start_ns_) / 1000; }
  std::int64_t sinceMarkUs(const std::string& name) const {
    auto it = marks_.find(name);
    const auto base = it == marks_.end() ? start_ns_ : it->second;
    return (steadyNs() - base) / 1000;
  }
private:
  std::int64_t start_ns_ = 0;
  std::unordered_map<std::string, std::int64_t> marks_;
};

} // namespace crashcore
