#pragma once
#include "timing/timestamp.hpp"
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <deque>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>
#include <unordered_map>
#include <cstdio>
#include <memory>

namespace crashcore {

enum class JsonLogLevel { Trace, Debug, Info, Warn, Error };
inline const char* toString(JsonLogLevel l) {
  switch (l) {
    case JsonLogLevel::Trace: return "trace";
    case JsonLogLevel::Debug: return "debug";
    case JsonLogLevel::Info: return "info";
    case JsonLogLevel::Warn: return "warn";
    case JsonLogLevel::Error: return "error";
  }
  return "info";
}

struct JsonLogEvent {
  JsonLogLevel level = JsonLogLevel::Info;
  std::string component, message, correlationId;
  std::unordered_map<std::string, std::string> fields;
  TimestampMs tsMs = 0;
};

class JsonLogger {
public:
  explicit JsonLogger(std::size_t ringCap = 4096) : ring_cap_(ringCap) {
    worker_ = std::jthread([this](std::stop_token st) { sinkLoop(st); });
  }
  ~JsonLogger() {
    running_.store(false); cv_.notify_all();
    if (worker_.joinable()) { worker_.request_stop(); worker_.join(); }
  }
  void setLevel(JsonLogLevel l) { level_.store(static_cast<int>(l)); }
  void setComponent(std::string c) { component_ = std::move(c); }
  void log(JsonLogLevel level, std::string message,
           std::unordered_map<std::string, std::string> fields = {},
           std::string correlationId = {}) {
    if (static_cast<int>(level) < level_.load()) return;
    JsonLogEvent ev;
    ev.level = level; ev.component = component_; ev.message = std::move(message);
    ev.correlationId = std::move(correlationId); ev.fields = std::move(fields); ev.tsMs = nowMs();
    { std::lock_guard lk(mu_);
      if (queue_.size() >= ring_cap_) { queue_.pop_front(); ++dropped_; }
      queue_.push_back(std::move(ev)); }
    cv_.notify_one();
  }
  void info(std::string msg, std::unordered_map<std::string, std::string> f = {}) {
    log(JsonLogLevel::Info, std::move(msg), std::move(f));
  }
  void error(std::string msg, std::unordered_map<std::string, std::string> f = {}) {
    log(JsonLogLevel::Error, std::move(msg), std::move(f));
  }
  std::uint64_t writtenCount() const noexcept { return written_.load(); }
  std::uint64_t droppedCount() const noexcept { return dropped_.load(); }
private:
  void sinkLoop(std::stop_token st) {
    while (!st.stop_requested() && running_.load()) {
      JsonLogEvent ev;
      { std::unique_lock lk(mu_);
        cv_.wait_for(lk, std::chrono::milliseconds(100), [&]{ return !queue_.empty() || st.stop_requested(); });
        if (queue_.empty()) continue;
        ev = std::move(queue_.front()); queue_.pop_front(); }
      writeLine(ev);
    }
  }
  void writeLine(const JsonLogEvent& ev) {
    std::ostringstream oss;
    oss << "{\"ts\":" << ev.tsMs << ",\"level\":\"" << toString(ev.level)
        << "\",\"component\":\"" << escape(ev.component) << "\",\"msg\":\"" << escape(ev.message) << "\"";
    if (!ev.correlationId.empty()) oss << ",\"correlationId\":\"" << escape(ev.correlationId) << "\"";
    for (const auto& [k,v] : ev.fields) oss << ",\"" << escape(k) << "\":\"" << escape(v) << "\"";
    oss << "}\n";
    auto line = oss.str();
    std::fwrite(line.data(), 1, line.size(), stderr);
    ++written_;
  }
  static std::string escape(const std::string& s) {
    std::string o; o.reserve(s.size());
    for (char c : s) {
      if (c=='"') o += "\\\""; else if (c=='\\') o += "\\\\"; else if (c=='\n') o += "\\n"; else o += c;
    }
    return o;
  }
  std::size_t ring_cap_;
  std::string component_;
  std::atomic<int> level_{static_cast<int>(JsonLogLevel::Info)};
  std::mutex mu_; std::condition_variable cv_;
  std::deque<JsonLogEvent> queue_;
  std::jthread worker_;
  std::atomic<bool> running_{true};
  std::atomic<std::uint64_t> dropped_{0}, written_{0};
};

inline JsonLogger& getLogger(const std::string& component) {
  static std::mutex mu;
  static std::unordered_map<std::string, std::unique_ptr<JsonLogger>> loggers;
  std::lock_guard lk(mu);
  auto& ptr = loggers[component];
  if (!ptr) { ptr = std::make_unique<JsonLogger>(); ptr->setComponent(component); }
  return *ptr;
}

} // namespace crashcore
