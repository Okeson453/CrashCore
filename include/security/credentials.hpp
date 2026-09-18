#pragma once
#include "security/secret_provider.hpp"
#include <string>

namespace crashcore {

struct TelegramCredentials {
  std::string botToken;
  std::string chatId;
  bool valid() const { return !botToken.empty() && !chatId.empty(); }
};

struct DatabaseCredentials {
  std::string connectionString;
  bool valid() const { return !connectionString.empty(); }
};

struct BcGameCredentials {
  std::string signKey;
  std::string userAgent;
  bool valid() const { return !signKey.empty(); }
};

inline TelegramCredentials loadTelegramCredentials(const SecretProvider& sp) {
  TelegramCredentials c;
  c.botToken = sp.get("TELEGRAM_BOT_TOKEN").value_or("");
  c.chatId   = sp.get("TELEGRAM_CHAT_ID").value_or("");
  return c;
}

inline DatabaseCredentials loadDatabaseCredentials(const SecretProvider& sp) {
  DatabaseCredentials c;
  c.connectionString = sp.get("DATABASE_URL").value_or("");
  return c;
}

inline BcGameCredentials loadBcGameCredentials(const SecretProvider& sp) {
  BcGameCredentials c;
  c.signKey   = sp.get("BC_SIGN_KEY").value_or("");
  c.userAgent = sp.get("BC_USER_AGENT").value_or("");
  return c;
}

} // namespace crashcore
