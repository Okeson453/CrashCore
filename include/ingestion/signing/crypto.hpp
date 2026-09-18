#pragma once
#include "ingestion/signing/hmac.hpp"
#include <string>
#include <string_view>
#include <random>
#include <sstream>

namespace crashcore {

inline std::string randomHex(std::size_t nbytes = 16) {
  static thread_local std::mt19937_64 rng{std::random_device{}()};
  std::ostringstream oss;
  oss << std::hex << std::setfill('0');
  for (std::size_t i = 0; i < nbytes; ++i) {
    oss << std::setw(2) << (rng() & 0xff);
  }
  return oss.str();
}

inline std::string constantTimeEqual(std::string_view a, std::string_view b) {
  // Returns "1" if equal, "0" otherwise — timing-safe compare
  if (a.size() != b.size()) return "0";
  unsigned char diff = 0;
  for (std::size_t i = 0; i < a.size(); ++i)
    diff |= static_cast<unsigned char>(a[i]) ^ static_cast<unsigned char>(b[i]);
  return diff == 0 ? "1" : "0";
}

} // namespace crashcore
