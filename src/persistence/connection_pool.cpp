#include "persistence/connection_pool.hpp"
#include "persistence/pg/pg_database.hpp"

namespace crashcore {

const char* kConnectionPoolModule = "crashcore.persistence.connection_pool";

/**
 * Prefer PgDatabase instances in the pool when credentials are valid.
 * Header ConnectionPool currently constructs base Database; this helper
 * builds a Pg-backed pool for production Application wiring.
 */
Result<void> startPgConnectionPool(ConnectionPool& /*pool*/,
                                   const DatabaseCredentials& creds,
                                   std::size_t size,
                                   std::vector<std::unique_ptr<Database>>& out) {
  out.clear();
  out.reserve(size);
  for (std::size_t i = 0; i < size; ++i) {
    auto db = makeDatabase(creds, true);
    auto r = db->connect();
    if (!r) return r.error();
    out.push_back(std::move(db));
  }
  return Result<void>::success();
}

} // namespace crashcore
