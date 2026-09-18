#pragma once
#include "security/secret_provider.hpp"
#include <string>
#include <vector>
#include <sstream>

namespace crashcore {

struct TelegramCredentials {
  std::string botToken;
  std::string chatId;                       // primary
  std::vector<std::string> extraChatIds;    // GROUP + EXTRA fan-out
  bool valid() const {
    return !botToken.empty() && (!chatId.empty() || !extraChatIds.empty());
  }
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

inline std::vector<std::string> splitCommaIds(const std::string& s) {
  std::vector<std::string> out;
  std::stringstream ss(s);
  std::string item;
  while (std::getline(ss, item, ',')) {
    // trim spaces
    while (!item.empty() && item.front() == ' ') item.erase(item.begin());
    while (!item.empty() && item.back() == ' ') item.pop_back();
    if (!item.empty()) out.push_back(item);
  }
  return out;
}

/**
 * Load Telegram credentials matching TestingEngine env contract:
 *   TELEGRAM_BOT_TOKEN (required)
 *   TELEGRAM_CHAT_ID (primary)
 *   TELEGRAM_GROUP_CHAT_ID (optional)
 *   TELEGRAM_EXTRA_CHAT_IDS (optional, comma-separated)
 */
inline TelegramCredentials loadTelegramCredentials(const SecretProvider& sp) {
  TelegramCredentials c;
  c.botToken = sp.get("TELEGRAM_BOT_TOKEN").value_or("");
  c.chatId   = sp.get("TELEGRAM_CHAT_ID").value_or("");
  auto group = sp.get("TELEGRAM_GROUP_CHAT_ID").value_or("");
  if (!group.empty()) c.extraChatIds.push_back(group);
  auto extra = sp.get("TELEGRAM_EXTRA_CHAT_IDS").value_or("");
  for (auto& id : splitCommaIds(extra)) c.extraChatIds.push_back(id);
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
