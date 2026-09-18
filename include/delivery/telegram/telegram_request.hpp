#pragma once
#include <string>

namespace crashcore {

struct TelegramRequest {
  std::string method = "sendMessage";
  std::string chatId;
  std::string text;
  std::string parseMode; // "HTML" | "Markdown" | empty
  bool disableNotification = false;
};

} // namespace crashcore
