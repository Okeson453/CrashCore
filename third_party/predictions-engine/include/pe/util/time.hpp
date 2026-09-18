#pragma once

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>

namespace pe::util {

/** Current UTC time as ISO-8601 string (YYYY-MM-DDTHH:MM:SS.sssZ) */
inline std::string nowIso8601() {
  using namespace std::chrono;
  const auto now = system_clock::now();
  const auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;
  const std::time_t t = system_clock::to_time_t(now);
  std::tm tm{};
#if defined(_WIN32)
  gmtime_s(&tm, &t);
#else
  gmtime_r(&t, &tm);
#endif
  std::ostringstream oss;
  oss << std::put_time(&tm, "%Y-%m-%dT%H:%M:%S")
      << '.' << std::setfill('0') << std::setw(3) << ms.count() << 'Z';
  return oss.str();
}

/** Milliseconds since epoch */
inline int64_t nowMs() {
  using namespace std::chrono;
  return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

/** Add seconds to an ISO-8601 timestamp and return new ISO string (approx). */
inline std::string expiresAtIso(int secondsFromNow = 30) {
  using namespace std::chrono;
  const auto then = system_clock::now() + seconds(secondsFromNow);
  const auto ms = duration_cast<milliseconds>(then.time_since_epoch()) % 1000;
  const std::time_t t = system_clock::to_time_t(then);
  std::tm tm{};
#if defined(_WIN32)
  gmtime_s(&tm, &t);
#else
  gmtime_r(&t, &tm);
#endif
  std::ostringstream oss;
  oss << std::put_time(&tm, "%Y-%m-%dT%H:%M:%S")
      << '.' << std::setfill('0') << std::setw(3) << ms.count() << 'Z';
  return oss.str();
}

} // namespace pe::util
