#pragma once
#include <cstdio>
#include <string>
#include <string_view>
#include <sstream>

namespace crashcore::telegram_api {

inline constexpr std::string_view HOST = "api.telegram.org";
inline constexpr int PORT = 443;

inline std::string sendMessagePath(std::string_view botToken) {
  return "/bot" + std::string(botToken) + "/sendMessage";
}

inline std::string getMePath(std::string_view botToken) {
  return "/bot" + std::string(botToken) + "/getMe";
}

inline std::string escapeJson(std::string_view text) {
  std::string out;
  out.reserve(text.size() + 8);
  for (char c : text) {
    switch (c) {
      case '"':  out += "\\\""; break;
      case '\\': out += "\\\\"; break;
      case '\n': out += "\\n"; break;
      case '\r': out += "\\r"; break;
      case '\t': out += "\\t"; break;
      default:
        if (static_cast<unsigned char>(c) < 0x20) {
          char buf[8];
          std::snprintf(buf, sizeof(buf), "\\u%04x", static_cast<unsigned>(c));
          out += buf;
        } else {
          out += c;
        }
    }
  }
  return out;
}

inline std::string buildSendMessageBody(std::string_view chatId, std::string_view text,
                                        std::string_view parseMode = {},
                                        bool disableNotification = false) {
  std::ostringstream oss;
  oss << "{\"chat_id\":\"" << escapeJson(chatId) << "\",\"text\":\"" << escapeJson(text) << "\"";
  if (!parseMode.empty()) {
    oss << ",\"parse_mode\":\"" << escapeJson(parseMode) << "\"";
  }
  if (disableNotification) {
    oss << ",\"disable_notification\":true";
  }
  oss << "}";
  return oss.str();
}

} // namespace crashcore::telegram_api
