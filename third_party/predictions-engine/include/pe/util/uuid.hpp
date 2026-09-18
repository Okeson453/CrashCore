#pragma once

#include <random>
#include <sstream>
#include <string>
#include <iomanip>

namespace pe::util {

/** RFC 4122 v4 UUID */
inline std::string randomUUID() {
  static thread_local std::mt19937_64 rng{std::random_device{}()};
  std::uniform_int_distribution<uint64_t> dist;
  uint64_t a = dist(rng);
  uint64_t b = dist(rng);
  // Set version (4) and variant (10xx)
  a = (a & 0xFFFFFFFFFFFF0FFFULL) | 0x0000000000004000ULL;
  b = (b & 0x3FFFFFFFFFFFFFFFULL) | 0x8000000000000000ULL;

  std::ostringstream oss;
  oss << std::hex << std::setfill('0')
      << std::setw(8) << ((a >> 32) & 0xFFFFFFFFULL) << '-'
      << std::setw(4) << ((a >> 16) & 0xFFFFULL) << '-'
      << std::setw(4) << (a & 0xFFFFULL) << '-'
      << std::setw(4) << ((b >> 48) & 0xFFFFULL) << '-'
      << std::setw(12) << (b & 0xFFFFFFFFFFFFULL);
  return oss.str();
}

} // namespace pe::util
