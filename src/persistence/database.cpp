#include "persistence/database.hpp"
#include "persistence/pg/pg_database.hpp"
namespace crashcore {
std::unique_ptr<Database> makeDatabase(const DatabaseCredentials& creds, bool preferPg) {
  if (preferPg) return std::make_unique<PgDatabase>(creds);
  return std::make_unique<Database>(creds);
}
} // namespace crashcore
