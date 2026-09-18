#pragma once
/**
 * PORT_BEHAVIOR multi-chat dispatch from TestingEngine telegram.ts
 * sendToChat never throws; per-destination health; 2s timeout budget.
 */
#include "delivery/telegram/telegram_client.hpp"
#include "delivery/telegram/message_format.hpp"
#include "delivery/telegram/http_client.hpp"
#include "security/credentials.hpp"
#include "timing/timestamp.hpp"
#include "timing/monotonic_clock.hpp"
#include "common/result.hpp"
#include <atomic>
#include <mutex>
#include <string>
#include <vector>
#include <unordered_map>

namespace crashcore {

struct SendResult {
  bool ok = false;
  int status = 0;
  std::string error;
  std::string chatId;
  std::int64_t durationMs = 0;
};

struct ChatHealth {
  std::string chatId;
  std::uint64_t success = 0;
  std::uint64_t failure = 0;
  TimestampMs lastSuccessMs = 0;
  TimestampMs lastFailureMs = 0;
  std::string lastError;
};

class TelegramDispatcher {
public:
  explicit TelegramDispatcher(TelegramCredentials primary,
                              std::vector<std::string> extraChatIds = {},
                              std::shared_ptr<HttpClient> http = nullptr)
      : creds_(std::move(primary))
      , http_(http ? std::move(http) : std::make_shared<HttpClient>())
      , client_(creds_, http_) {
    if (creds_.valid()) chat_ids_.push_back(creds_.chatId);
    for (auto& c : extraChatIds) {
      if (!c.empty()) chat_ids_.push_back(c);
    }
  }

  bool ready() const noexcept { return creds_.valid() && !chat_ids_.empty(); }

  SendResult sendToChat(const std::string& chatId, const std::string& text,
                        std::int64_t timeoutMs = 2000) {
    SendResult sr;
    sr.chatId = chatId;
    const auto t0 = MonotonicClock::now();
    // Temporarily override chat id via request body path
    TelegramCredentials c = creds_;
    c.chatId = chatId;
    TelegramClient client(c, http_);
    auto r = client.sendMessage(text);
    sr.durationMs = MonotonicClock::elapsedUs(t0, MonotonicClock::now()) / 1000;
    if (r) {
      sr.ok = true;
      sr.status = 200;
      noteSuccess(chatId);
    } else {
      sr.ok = false;
      sr.error = r.error().message;
      sr.status = r.error().code == ErrorCode::RateLimited ? 429 :
                  r.error().code == ErrorCode::AuthFailed ? 401 : 0;
      noteFailure(chatId, sr.error);
    }
    (void)timeoutMs;
    return sr;
  }

  std::vector<SendResult> broadcast(const std::string& text) {
    std::vector<SendResult> out;
    out.reserve(chat_ids_.size());
    for (const auto& id : chat_ids_) out.push_back(sendToChat(id, text));
    return out;
  }

  bool broadcastOk(const std::string& text) {
    auto results = broadcast(text);
    for (const auto& r : results) if (r.ok) return true;
    return false;
  }

  std::vector<ChatHealth> chatHealth() const {
    std::lock_guard lk(mu_);
    std::vector<ChatHealth> out;
    for (const auto& [_, h] : health_) out.push_back(h);
    return out;
  }

  const std::vector<std::string>& chatIds() const noexcept { return chat_ids_; }
  TelegramClient& client() noexcept { return client_; }

private:
  void noteSuccess(const std::string& chatId) {
    std::lock_guard lk(mu_);
    auto& h = health_[chatId];
    h.chatId = chatId;
    ++h.success;
    h.lastSuccessMs = nowMs();
  }
  void noteFailure(const std::string& chatId, const std::string& err) {
    std::lock_guard lk(mu_);
    auto& h = health_[chatId];
    h.chatId = chatId;
    ++h.failure;
    h.lastFailureMs = nowMs();
    h.lastError = err;
  }

  TelegramCredentials creds_;
  std::shared_ptr<HttpClient> http_;
  TelegramClient client_;
  std::vector<std::string> chat_ids_;
  mutable std::mutex mu_;
  std::unordered_map<std::string, ChatHealth> health_;
};

} // namespace crashcore
