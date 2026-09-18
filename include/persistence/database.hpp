#pragma once
#include <memory>
#include "common/result.hpp"
#include "security/credentials.hpp"
#include <string>
#include <atomic>

namespace crashcore {

class Database {
public:
  explicit Database(DatabaseCredentials creds) : creds_(std::move(creds)) {}
  virtual ~Database() = default;

  virtual Result<void> connect() {
    if (!creds_.valid()) return Error{ErrorCode::ConfigError, "DATABASE_URL missing"};
    connected_.store(true);
    return Result<void>::success();
  }

  virtual void disconnect() { connected_.store(false); }
  bool connected() const noexcept { return connected_.load(); }

  virtual Result<void> execute(const std::string& /*sql*/) {
    if (!connected()) return Error{ErrorCode::DatabaseError, "not connected"};
    ++executions_;
    return Result<void>::success();
  }

  std::uint64_t executionCount() const noexcept {
    return executions_.load(std::memory_order_relaxed);
  }

protected:
  DatabaseCredentials creds_;
  std::atomic<bool> connected_{false};
  std::atomic<std::uint64_t> executions_{0};
};


std::unique_ptr<Database> makeDatabase(const DatabaseCredentials& creds, bool preferPg = true);

} // namespace crashcore
