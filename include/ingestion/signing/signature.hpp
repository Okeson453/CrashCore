#pragma once
#include "common/types.hpp"
#include <string>

namespace crashcore {

struct SocketSignature {
  std::string p;       // signature parameter
  std::string t;       // timestamp parameter
  std::string ua;      // user-agent used
  TimestampMs at_ms = 0;
  bool valid = false;
};

} // namespace crashcore
