#pragma once

#include <chrono>
#include <cmath>
#include <ctime>
#include <string>
#include <unordered_map>

namespace pe {

inline std::unordered_map<std::string, double> computeTimeFeatures(
    std::chrono::system_clock::time_point at = std::chrono::system_clock::now()) {
  const std::time_t t = std::chrono::system_clock::to_time_t(at);
  std::tm tm{};
#if defined(_WIN32)
  gmtime_s(&tm, &t);
#else
  gmtime_r(&t, &tm);
#endif
  const double hour = tm.tm_hour + tm.tm_min / 60.0;
  const int dow = tm.tm_wday; // 0 = Sunday
  return {
    {"hour_sin", std::sin(2.0 * 3.14159265358979323846 * hour / 24.0)},
    {"hour_cos", std::cos(2.0 * 3.14159265358979323846 * hour / 24.0)},
    {"dow_sin", std::sin(2.0 * 3.14159265358979323846 * dow / 7.0)},
    {"dow_cos", std::cos(2.0 * 3.14159265358979323846 * dow / 7.0)},
    {"hour_utc", hour},
    {"is_weekend", (dow == 0 || dow == 6) ? 1.0 : 0.0},
  };
}

} // namespace pe
