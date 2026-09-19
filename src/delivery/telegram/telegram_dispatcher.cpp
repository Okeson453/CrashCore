#include "delivery/telegram/telegram_dispatcher.hpp"

namespace crashcore {

const char* kTelegramDispatcherModule = "crashcore.delivery.telegram_dispatcher";

bool telegramBroadcastOk(TelegramDispatcher& d, const std::string& text) {
  return d.broadcastOk(text);
}

std::size_t telegramChatCount(const TelegramDispatcher& d) {
  return d.chatIds().size();
}

} // namespace crashcore
