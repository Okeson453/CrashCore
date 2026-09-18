#pragma once
#include "logging/logger.hpp"
#include <sstream>
#include <string>

namespace crashcore {

class StructuredLogger {
public:
  explicit StructuredLogger(ComponentId comp) : comp_(comp) {}

  template <typename... Args>
  void info(Args&&... args) {
    log(LogLevel::Info, std::forward<Args>(args)...);
  }
  template <typename... Args>
  void warn(Args&&... args) {
    log(LogLevel::Warn, std::forward<Args>(args)...);
  }
  template <typename... Args>
  void error(Args&&... args) {
    log(LogLevel::Error, std::forward<Args>(args)...);
  }
  template <typename... Args>
  void debug(Args&&... args) {
    log(LogLevel::Debug, std::forward<Args>(args)...);
  }

private:
  template <typename T>
  static void append(std::ostringstream& oss, T&& v) {
    oss << std::forward<T>(v);
  }
  template <typename T, typename... Rest>
  static void append(std::ostringstream& oss, T&& v, Rest&&... rest) {
    oss << std::forward<T>(v);
    append(oss, std::forward<Rest>(rest)...);
  }
  template <typename... Args>
  void log(LogLevel lvl, Args&&... args) {
    std::ostringstream oss;
    append(oss, std::forward<Args>(args)...);
    logger().log(lvl, comp_, oss.str());
  }

  ComponentId comp_;
};

} // namespace crashcore
