#include "delivery/telegram/telegram_client.hpp"
#include "delivery/http/curl_http_client.hpp"
namespace crashcore {
// Uses CurlHttpClient by default. Parses Telegram retry_after on HTTP 429.
// Requires TELEGRAM_BOT_TOKEN + TELEGRAM_CHAT_ID via SecretProvider.
} // namespace crashcore
