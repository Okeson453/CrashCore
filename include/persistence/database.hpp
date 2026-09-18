#pragma once
#include <memory>
#include <vector>
#include "common/result.hpp"
#include "security/credentials.hpp"
#include <string>
#include <atomic>

namespace crashcore {

/** Simple tabular result shared by base Database and PgDatabase. */
struct QueryResult {
  std::vector<std::vector<std::string>> rows;
  std::vector<std::string> columns;
};

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

  /**
   * Run a SELECT (or any query returning tuples).
   * Base stub returns empty result so unit tests without Postgres still compile.
   * PgDatabase overrides with real libpq.
   */
  virtual Result<QueryResult> query(const std::string& /*sql*/,
                                    const std::vector<std::string>& /*params*/ = {}) {
    if (!connected()) return Error{ErrorCode::DatabaseError, "not connected"};
    ++executions_;
    return QueryResult{};
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
