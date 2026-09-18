#pragma once
#include "persistence/database.hpp"
#include "persistence/pg/pg_database.hpp"
#include "persistence/schema.hpp"
#include "common/result.hpp"
#include "timing/timestamp.hpp"
#include <atomic>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>

namespace crashcore {

class MigrationRunner {
public:
  explicit MigrationRunner(Database& db, std::string migrationsDir = "migrations")
      : db_(db), dir_(std::move(migrationsDir)) {}

  Result<void> ensureTable() {
    return db_.execute(
      "CREATE TABLE IF NOT EXISTS schema_migrations ("
      "id TEXT PRIMARY KEY, checksum TEXT NOT NULL, applied_at_ms BIGINT NOT NULL)");
  }

  Result<void> applyAll() {
    auto t = ensureTable();
    if (!t) return t;
    // Prefer filesystem migrations/
    std::vector<std::filesystem::path> files;
    std::error_code ec;
    if (std::filesystem::is_directory(dir_, ec)) {
      for (auto& e : std::filesystem::directory_iterator(dir_, ec)) {
        if (e.path().extension() == ".sql") files.push_back(e.path());
      }
      std::sort(files.begin(), files.end());
    }
    if (!files.empty()) {
      for (auto& f : files) {
        auto id = f.filename().string();
        if (isApplied(id)) continue;
        std::ifstream in(f);
        if (!in) return Error{ErrorCode::IoError, "cannot read " + id};
        std::ostringstream ss; ss << in.rdbuf();
        auto r = applySql(id, ss.str());
        if (!r) return r;
      }
      return Result<void>::success();
    }
    // Fallback: embedded schema.hpp
    for (auto sv : schema::allMigrations()) {
      auto id = std::string("embedded_") + std::to_string(applied_.load());
      auto r = applySql(id, std::string(sv));
      if (!r) return r;
    }
    return Result<void>::success();
  }

  Result<void> applyBuiltinSchema() { return applyAll(); }

  std::uint64_t appliedCount() const noexcept { return applied_.load(); }

private:
  bool isApplied(const std::string& id) {
    auto* pg = dynamic_cast<PgDatabase*>(&db_);
    if (pg) {
      auto qr = pg->query("SELECT 1 FROM schema_migrations WHERE id=$1", {id});
      return qr && !qr.value().rows.empty();
    }
    return false;
  }

  Result<void> applySql(const std::string& id, const std::string& sql) {
    auto* pg = dynamic_cast<PgDatabase*>(&db_);
    if (pg) {
      auto b = pg->execute("BEGIN");
      if (!b) return b;
    }
    // split statements
    std::size_t start = 0;
    while (start < sql.size()) {
      auto end = sql.find(';', start);
      std::string stmt = end == std::string::npos ? sql.substr(start) : sql.substr(start, end - start);
      while (!stmt.empty() && (stmt.front()==' '||stmt.front()=='\n'||stmt.front()=='\t'||stmt.front()=='\r'))
        stmt.erase(stmt.begin());
      // skip comment-only
      if (!stmt.empty() && stmt.find_first_not_of(" \t\n\r-") != std::string::npos) {
        auto r = db_.execute(stmt);
        if (!r) {
          if (pg) pg->execute("ROLLBACK");
          return r;
        }
      }
      if (end == std::string::npos) break;
      start = end + 1;
    }
    if (pg) {
      auto rec = pg->executeParams(
        "INSERT INTO schema_migrations(id,checksum,applied_at_ms) VALUES($1,$2,$3) ON CONFLICT DO NOTHING",
        {id, "sha-pending", std::to_string(nowMs())});
      if (!rec) { pg->execute("ROLLBACK"); return rec; }
      pg->execute("COMMIT");
    }
    ++applied_;
    return Result<void>::success();
  }

  Database& db_;
  std::string dir_;
  std::atomic<std::uint64_t> applied_{0};
};

} // namespace crashcore
