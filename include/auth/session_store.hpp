#pragma once
#include "timing/timestamp.hpp"
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>
namespace crashcore {
struct Session {
  std::string id;
  std::string userId;
  TimestampMs expiresAtMs=0;
  std::string role="viewer";
};
class SessionStore {
public:
  std::string create(const std::string& userId, const std::string& role, std::int64_t ttlMs=86400000) {
    std::lock_guard lk(mu_);
    Session s; s.id = userId + "-" + std::to_string(nowMs());
    s.userId=userId; s.role=role; s.expiresAtMs=nowMs()+ttlMs;
    sessions_[s.id]=s; return s.id;
  }
  std::optional<Session> get(const std::string& id) const {
    std::lock_guard lk(mu_);
    auto it=sessions_.find(id); if(it==sessions_.end()) return std::nullopt;
    if (it->second.expiresAtMs < nowMs()) return std::nullopt;
    return it->second;
  }
  void revoke(const std::string& id) { std::lock_guard lk(mu_); sessions_.erase(id); }
  bool authorize(const std::string& id, const std::string& minRole) const {
    auto s=get(id); if(!s) return false;
    if (minRole=="viewer") return true;
    if (minRole=="operator") return s->role=="operator"||s->role=="admin";
    if (minRole=="admin") return s->role=="admin";
    return false;
  }
private:
  mutable std::mutex mu_;
  std::unordered_map<std::string,Session> sessions_;
};
} // namespace crashcore
