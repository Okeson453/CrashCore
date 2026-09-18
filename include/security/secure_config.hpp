#pragma once
#include "security/secret_provider.hpp"
#include "security/credentials.hpp"
#include "common/result.hpp"
#include <string>

namespace crashcore {

struct SecureConfig {
  TelegramCredentials telegram;
  DatabaseCredentials database;
  BcGameCredentials   bcgame;
  std::string         logLevel = "info";
  bool                production = false;
};

inline Result<SecureConfig> loadSecureConfig(SecretProvider& sp) {
  SecureConfig cfg;
  cfg.telegram = loadTelegramCredentials(sp);
  cfg.database = loadDatabaseCredentials(sp);
  cfg.bcgame   = loadBcGameCredentials(sp);
  if (auto v = sp.get("LOG_LEVEL")) cfg.logLevel = *v;
  if (auto v = sp.get("NODE_ENV")) cfg.production = (*v == "production");
  return cfg;
}

} // namespace crashcore
