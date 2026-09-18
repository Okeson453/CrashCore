#pragma once
#include "common/enums.hpp"
#include "timing/timestamp.hpp"
#include <atomic>
#include <chrono>
#include <cstdio>
#include <mutex>
#include <string>
#include <string_view>
#include <sstream>
#include <iostream>

namespace crashcore {

class Logger {
public:
  static Logger& instance() {
    static Logger inst;
    return inst;
  }

  void setLevel(LogLevel level) noexcept { level_.store(level, std::memory_order_relaxed); }
  LogLevel level() const noexcept { return level_.load(std::memory_order_relaxed); }

  void log(LogLevel lvl, ComponentId comp, std::string_view msg) {
    if (lvl < level()) return;
    const auto ms = nowMs();
    std::lock_guard lk(mu_);
    std::fprintf(stderr, "%lld [%s] [%s] %.*s\n",
                 static_cast<long long>(ms),
                 toString(lvl).data(),
                 toString(comp).data(),
                 static_cast<int>(msg.size()), msg.data());
  }

  void trace(ComponentId c, std::string_view m) { log(LogLevel::Trace, c, m); }
  void debug(ComponentId c, std::string_view m) { log(LogLevel::Debug, c, m); }
  void info(ComponentId c, std::string_view m)  { log(LogLevel::Info, c, m); }
  void warn(ComponentId c, std::string_view m)  { log(LogLevel::Warn, c, m); }
  void error(ComponentId c, std::string_view m) { log(LogLevel::Error, c, m); }
  void fatal(ComponentId c, std::string_view m) { log(LogLevel::Fatal, c, m); }

private:
  Logger() = default;
  std::atomic<LogLevel> level_{LogLevel::Info};
  std::mutex mu_;
};

inline Logger& logger() { return Logger::instance(); }

} // namespace crashcore
