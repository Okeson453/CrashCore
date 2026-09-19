#include "delivery/telegram/telegram_client.hpp"

namespace crashcore {

const char* kTelegramClientModule = "crashcore.delivery.telegram_client";

bool telegramClientReady(const TelegramClient& c) {
  return c.ready();
}

} // namespace crashcore
