#pragma once
/**
 * libpq-backed Database implementation.
 * connect / execute / query with parameterized support via PQexecParams.
 */
#include "persistence/database.hpp"
#include "common/result.hpp"
#include "common/errors.hpp"
#include <libpq-fe.h>
#include <vector>
#include <memory>
#include <mutex>
#include <cstring>

namespace crashcore {

class PgDatabase : public Database {
public:
  explicit PgDatabase(DatabaseCredentials creds) : Database(std::move(creds)) {}

  ~PgDatabase() override {
    disconnect();
  }

  Result<void> connect() override {
    std::lock_guard lk(mu_);
    if (conn_ && PQstatus(conn_) == CONNECTION_OK) {
      connected_.store(true);
      return Result<void>::success();
    }
    if (conn_) {
      PQfinish(conn_);
      conn_ = nullptr;
    }
    if (!creds_.valid()) {
      return Error{ErrorCode::ConfigError, "DATABASE_URL / credentials missing"};
    }

    const std::string& conninfo = creds_.connectionString;
    conn_ = PQconnectdb(conninfo.c_str());
    if (PQstatus(conn_) != CONNECTION_OK) {
      std::string err = PQerrorMessage(conn_);
      PQfinish(conn_);
      conn_ = nullptr;
      connected_.store(false);
      return Error{ErrorCode::DatabaseError, "PQconnectdb: " + err};
    }
    connected_.store(true);
    return Result<void>::success();
  }

  void disconnect() override {
    std::lock_guard lk(mu_);
    if (conn_) {
      PQfinish(conn_);
      conn_ = nullptr;
    }
    connected_.store(false);
  }

  Result<void> execute(const std::string& sql) override {
    std::lock_guard lk(mu_);
    if (!conn_ || PQstatus(conn_) != CONNECTION_OK) {
      return Error{ErrorCode::DatabaseError, "not connected"};
    }
    PGresult* res = PQexec(conn_, sql.c_str());
    const ExecStatusType st = PQresultStatus(res);
    if (st != PGRES_COMMAND_OK && st != PGRES_TUPLES_OK) {
      std::string err = PQerrorMessage(conn_);
      PQclear(res);
      ++failures_;
      return Error{ErrorCode::DatabaseError, err};
    }
    PQclear(res);
    ++executions_;
    return Result<void>::success();
  }

  /** Parameterized execute: $1..$N style. */
  Result<void> executeParams(const std::string& sql,
                             const std::vector<std::string>& params) {
    std::lock_guard lk(mu_);
    if (!conn_ || PQstatus(conn_) != CONNECTION_OK) {
      return Error{ErrorCode::DatabaseError, "not connected"};
    }
    std::vector<const char*> values;
    values.reserve(params.size());
    for (const auto& p : params) values.push_back(p.c_str());

    PGresult* res = PQexecParams(
        conn_, sql.c_str(),
        static_cast<int>(params.size()),
        nullptr, values.data(), nullptr, nullptr, 0);

    const ExecStatusType st = PQresultStatus(res);
    if (st != PGRES_COMMAND_OK && st != PGRES_TUPLES_OK) {
      std::string err = PQerrorMessage(conn_);
      PQclear(res);
      ++failures_;
      return Error{ErrorCode::DatabaseError, err};
    }
    PQclear(res);
    ++executions_;
    return Result<void>::success();
  }

  struct QueryResult {
    std::vector<std::vector<std::string>> rows;
    std::vector<std::string> columns;
  };

  Result<QueryResult> query(const std::string& sql,
                            const std::vector<std::string>& params = {}) {
    std::lock_guard lk(mu_);
    if (!conn_ || PQstatus(conn_) != CONNECTION_OK) {
      return Error{ErrorCode::DatabaseError, "not connected"};
    }
    std::vector<const char*> values;
    values.reserve(params.size());
    for (const auto& p : params) values.push_back(p.c_str());

    PGresult* res = PQexecParams(
        conn_, sql.c_str(),
        static_cast<int>(params.size()),
        nullptr,
        params.empty() ? nullptr : values.data(),
        nullptr, nullptr, 0);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
      std::string err = PQerrorMessage(conn_);
      PQclear(res);
      ++failures_;
      return Error{ErrorCode::DatabaseError, err};
    }

    QueryResult qr;
    const int nfields = PQnfields(res);
    const int ntuples = PQntuples(res);
    qr.columns.resize(static_cast<std::size_t>(nfields));
    for (int i = 0; i < nfields; ++i) {
      qr.columns[static_cast<std::size_t>(i)] = PQfname(res, i);
    }
    qr.rows.resize(static_cast<std::size_t>(ntuples));
    for (int r = 0; r < ntuples; ++r) {
      qr.rows[static_cast<std::size_t>(r)].resize(static_cast<std::size_t>(nfields));
      for (int c = 0; c < nfields; ++c) {
        if (PQgetisnull(res, r, c)) {
          qr.rows[static_cast<std::size_t>(r)][static_cast<std::size_t>(c)] = "";
        } else {
          qr.rows[static_cast<std::size_t>(r)][static_cast<std::size_t>(c)] =
              PQgetvalue(res, r, c);
        }
      }
    }
    PQclear(res);
    ++executions_;
    return qr;
  }

  /** Health check — simple SELECT 1. */
  bool healthy() {
    auto r = execute("SELECT 1");
    return static_cast<bool>(r);
  }

  std::uint64_t failureCount() const noexcept {
    return failures_.load(std::memory_order_relaxed);
  }

  PGconn* raw() noexcept { return conn_; }

private:
  std::mutex mu_;
  PGconn* conn_ = nullptr;
  std::atomic<std::uint64_t> failures_{0};
};

} // namespace crashcore
