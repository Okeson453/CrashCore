#pragma once
#include <string>
#include <cstdint>

namespace crashcore {

struct TelegramResponse {
  bool ok = false;
  std::int64_t messageId = 0;
  std::string description;
  std::string raw;
  int errorCode = 0;
};

} // namespace crashcore
