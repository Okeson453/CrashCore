#pragma once
#include "persistence/database.hpp"
#include "common/result.hpp"

namespace crashcore {

class Transaction {
public:
  explicit Transaction(Database& db) : db_(db) { db_.execute("BEGIN"); }
  ~Transaction() { if (!done_) db_.execute("ROLLBACK"); }
  Result<void> commit() { auto r = db_.execute("COMMIT"); done_ = true; return r; }
  Result<void> rollback() { auto r = db_.execute("ROLLBACK"); done_ = true; return r; }
private:
  Database& db_;
  bool done_ = false;
};

} // namespace crashcore
