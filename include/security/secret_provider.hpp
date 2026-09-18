#pragma once
#include <optional>
#include <string>
#include <unordered_map>
#include <mutex>
#include <cstdlib>

namespace crashcore {

/**
 * Abstraction over secrets (env, file, vault).
 * Never log secret values.
 */
class SecretProvider {
public:
  virtual ~SecretProvider() = default;
  virtual std::optional<std::string> get(const std::string& key) const = 0;
  virtual void set(const std::string& key, std::string value) = 0;
};

class EnvSecretProvider : public SecretProvider {
public:
  std::optional<std::string> get(const std::string& key) const override {
    {
      std::lock_guard lk(mu_);
      auto it = overrides_.find(key);
      if (it != overrides_.end()) return it->second;
    }
    const char* v = std::getenv(key.c_str());
    if (v && *v) return std::string(v);
    return std::nullopt;
  }

  void set(const std::string& key, std::string value) override {
    std::lock_guard lk(mu_);
    overrides_[key] = std::move(value);
  }

private:
  mutable std::mutex mu_;
  std::unordered_map<std::string, std::string> overrides_;
};

class MapSecretProvider : public SecretProvider {
public:
  explicit MapSecretProvider(std::unordered_map<std::string, std::string> m)
      : map_(std::move(m)) {}

  std::optional<std::string> get(const std::string& key) const override {
    std::lock_guard lk(mu_);
    auto it = map_.find(key);
    if (it == map_.end()) return std::nullopt;
    return it->second;
  }

  void set(const std::string& key, std::string value) override {
    std::lock_guard lk(mu_);
    map_[key] = std::move(value);
  }

private:
  mutable std::mutex mu_;
  std::unordered_map<std::string, std::string> map_;
};

} // namespace crashcore
