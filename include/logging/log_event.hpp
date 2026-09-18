#pragma once
#include "common/enums.hpp"
#include "common/types.hpp"
#include <string>

namespace crashcore {

struct LogEvent {
  LogLevel      level = LogLevel::Info;
  ComponentId   component = ComponentId::Application;
  TimestampMs   timestampMs = 0;
  std::string   message;
  std::string   correlationId;
};

} // namespace crashcore
