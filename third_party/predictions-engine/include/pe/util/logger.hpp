#pragma once

#include <iostream>
#include <string>

namespace pe::util {

enum class LogLevel { Debug, Info, Warn, Error };

inline void log(LogLevel level, const std::string& msg) {
  const char* tag = "INFO";
  switch (level) {
    case LogLevel::Debug: tag = "DEBUG"; break;
    case LogLevel::Info:  tag = "INFO";  break;
    case LogLevel::Warn:  tag = "WARN";  break;
    case LogLevel::Error: tag = "ERROR"; break;
  }
  std::cerr << "[pe][" << tag << "] " << msg << std::endl;
}

inline void debug(const std::string& msg) { log(LogLevel::Debug, msg); }
inline void info(const std::string& msg)  { log(LogLevel::Info, msg); }
inline void warn(const std::string& msg)  { log(LogLevel::Warn, msg); }
inline void error(const std::string& msg) { log(LogLevel::Error, msg); }

} // namespace pe::util
