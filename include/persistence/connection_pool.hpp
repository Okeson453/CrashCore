#pragma once
#include "persistence/database.hpp"
#include "common/result.hpp"
#include <memory>
#include <vector>
#include <mutex>
#include <condition_variable>

namespace crashcore {

class ConnectionPool {
public:
  ConnectionPool(DatabaseCredentials creds, std::size_t size = 4)
      : creds_(std::move(creds)), size_(size) {}

  Result<void> start() {
    std::lock_guard lk(mu_);
    for (std::size_t i = 0; i < size_; ++i) {
      auto db = std::make_unique<Database>(creds_);
      auto r = db->connect();
      if (!r) return r.error();
      pool_.push_back(std::move(db));
    }
    available_ = pool_.size();
    return Result<void>::success();
  }

  void stop() {
    std::lock_guard lk(mu_);
    for (auto& d : pool_) if (d) d->disconnect();
    pool_.clear();
    available_ = 0;
  }

  std::unique_ptr<Database> acquire() {
    std::unique_lock lk(mu_);
    cv_.wait(lk, [&] { return available_ > 0 || pool_.empty(); });
    if (pool_.empty()) return nullptr;
    for (auto& d : pool_) {
      if (d) {
        auto out = std::move(d);
        --available_;
        return out;
      }
    }
    return nullptr;
  }

  void release(std::unique_ptr<Database> db) {
    if (!db) return;
    std::lock_guard lk(mu_);
    for (auto& slot : pool_) {
      if (!slot) {
        slot = std::move(db);
        ++available_;
        cv_.notify_one();
        return;
      }
    }
    pool_.push_back(std::move(db));
    ++available_;
    cv_.notify_one();
  }

  std::size_t available() const {
    std::lock_guard lk(mu_);
    return available_;
  }

private:
  DatabaseCredentials creds_;
  std::size_t size_;
  std::vector<std::unique_ptr<Database>> pool_;
  std::size_t available_ = 0;
  mutable std::mutex mu_;
  std::condition_variable cv_;
};

} // namespace crashcore
