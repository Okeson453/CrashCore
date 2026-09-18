#pragma once
/**
 * Minimal TOML-like config loader for CrashCore.
 * Supports key = "value" | number | true/false under [section] headers.
 */
#include "application/application_config.hpp"
#include "common/result.hpp"
#include "common/errors.hpp"
#include <fstream>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <cctype>

namespace crashcore {

class TomlConfig {
public:
  using Section = std::unordered_map<std::string, std::string>;

  Result<void> loadFile(const std::string& path) {
    std::ifstream in(path);
    if (!in) return Error{ErrorCode::ConfigError, "cannot open " + path};
    std::ostringstream ss;
    ss << in.rdbuf();
    return loadString(ss.str());
  }

  Result<void> loadString(std::string_view text) {
    sections_.clear();
    std::string current = "global";
    std::string line;
    std::istringstream stream{std::string(text)};
    while (std::getline(stream, line)) {
      auto hash = line.find('#');
      if (hash != std::string::npos) line.resize(hash);
      auto l = trim(line);
      if (l.empty()) continue;
      if (l.front() == '[' && l.back() == ']') {
        current = std::string(l.substr(1, l.size() - 2));
        continue;
      }
      auto eq = l.find('=');
      if (eq == std::string::npos) continue;
      auto key = trim(l.substr(0, eq));
      auto val = trim(l.substr(eq + 1));
      if (val.size() >= 2 && ((val.front() == '"' && val.back() == '"') ||
                              (val.front() == '\'' && val.back() == '\''))) {
        val = val.substr(1, val.size() - 2);
      }
      sections_[current][key] = val;
    }
    return Result<void>::success();
  }

  std::string get(std::string_view section, std::string_view key,
                  std::string_view fallback = {}) const {
    auto sit = sections_.find(std::string(section));
    if (sit == sections_.end()) return std::string(fallback);
    auto kit = sit->second.find(std::string(key));
    if (kit == sit->second.end()) return std::string(fallback);
    return kit->second;
  }

  std::int64_t getInt(std::string_view section, std::string_view key,
                      std::int64_t fallback = 0) const {
    auto s = get(section, key);
    if (s.empty()) return fallback;
    try { return std::stoll(s); } catch (...) { return fallback; }
  }

  bool getBool(std::string_view section, std::string_view key, bool fallback = false) const {
    auto s = get(section, key);
    if (s.empty()) return fallback;
    if (s == "true" || s == "1" || s == "yes") return true;
    if (s == "false" || s == "0" || s == "no") return false;
    return fallback;
  }

  ApplicationConfig toApplicationConfig() const {
    ApplicationConfig cfg;
    cfg.host = get("ingestion", "host", cfg.host);
    cfg.path = get("ingestion", "path", cfg.path);
    cfg.nsp = get("ingestion", "nsp", cfg.nsp);
    cfg.eventQueueCapacity = static_cast<std::size_t>(
        getInt("concurrency", "event_queue_capacity",
               static_cast<std::int64_t>(cfg.eventQueueCapacity)));
    cfg.workerCount = static_cast<std::size_t>(
        getInt("concurrency", "worker_count",
               static_cast<std::int64_t>(cfg.workerCount)));
    cfg.outboxCapacity = static_cast<std::size_t>(
        getInt("delivery", "outbox_capacity",
               static_cast<std::int64_t>(cfg.outboxCapacity)));
    cfg.predictionTimeoutMs = getInt("prediction", "timeout_ms", cfg.predictionTimeoutMs);
    cfg.heartbeatIntervalMs = getInt("ingestion", "heartbeat_ms", cfg.heartbeatIntervalMs);
    cfg.enableTelegram = getBool("delivery", "enable_telegram", cfg.enableTelegram);
    cfg.enablePersistence = getBool("persistence", "enabled", cfg.enablePersistence);
    cfg.logLevel = get("logging", "level", cfg.logLevel);
    cfg.degradedMs = getInt("ingestion", "degraded_ms", cfg.degradedMs);
    cfg.staleMs = getInt("ingestion", "stale_ms", cfg.staleMs);
    cfg.stuckMaxAgeMs = getInt("validation", "stuck_max_age_ms", cfg.stuckMaxAgeMs);
    cfg.lossCooldownMs = getInt("validation", "loss_cooldown_ms", cfg.lossCooldownMs);
    cfg.lossCooldownThreshold = static_cast<int>(getInt("validation", "loss_cooldown_threshold", cfg.lossCooldownThreshold));
    cfg.enableNativeSocket = getBool("ingestion", "enable_native_socket", cfg.enableNativeSocket);
    cfg.socketHost = get("ingestion", "socket_host", cfg.socketHost);
    return cfg;
  }

  const std::unordered_map<std::string, Section>& sections() const noexcept {
    return sections_;
  }

private:
  static std::string trim(std::string_view s) {
    std::size_t b = 0, e = s.size();
    while (b < e && std::isspace(static_cast<unsigned char>(s[b]))) ++b;
    while (e > b && std::isspace(static_cast<unsigned char>(s[e - 1]))) --e;
    return std::string(s.substr(b, e - b));
  }

  std::unordered_map<std::string, Section> sections_;
};

} // namespace crashcore
