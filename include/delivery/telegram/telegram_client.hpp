#pragma once
/**
 * Telegram Bot API client (sendMessage).
 * Behaviour from TestingEngine notifications/telegram.ts:
 * request construction, retry, rate-limit awareness, timeout.
 * Off the critical prediction path.
 */
#include "delivery/telegram/telegram_request.hpp"
#include "delivery/telegram/telegram_response.hpp"
#include "delivery/telegram/telegram_api.hpp"
#include "delivery/telegram/http_client.hpp"
#include "delivery/http/curl_http_client.hpp"
#include "security/credentials.hpp"
#include "common/result.hpp"
#include "common/constants.hpp"
#include "timing/timestamp.hpp"
#include <atomic>
#include <memory>
#include <string>
#include <chrono>
#include <thread>

namespace crashcore {

class TelegramClient {
public:
  explicit TelegramClient(TelegramCredentials creds,
                          std::shared_ptr<HttpClient> http = nullptr)
      : creds_(std::move(creds))
      , http_(http ? std::move(http) : std::shared_ptr<HttpClient>(makeHttpClient(true))) {}

  virtual ~TelegramClient() = default;

  bool ready() const noexcept { return creds_.valid(); }

  Result<TelegramResponse> sendMessage(const std::string& text,
                                       std::int64_t timeoutMs = -1) {
    if (!creds_.valid()) {
      return Error{ErrorCode::AuthFailed, "telegram credentials missing"};
    }
    HttpRequest req;
    req.method = "POST";
    req.host = std::string(telegram_api::HOST);
    req.port = telegram_api::PORT;
    req.tls = true;
    req.path = telegram_api::sendMessagePath(creds_.botToken);
    req.body = telegram_api::buildSendMessageBody(creds_.chatId, text);
    req.headers["Content-Type"] = "application/json";
    req.headers["User-Agent"] = "CrashCore/1.0";
    req.timeout_ms = timeoutMs > 0 ? timeoutMs : constants::TELEGRAM_TIMEOUT_MS;

    auto httpResp = http_->request(req);
    if (!httpResp) {
      ++failed_;
      return httpResp.error();
    }
    return parseResponse(httpResp.value());
  }

  Result<TelegramResponse> sendMessageWithRetry(
      const std::string& text,
      int maxRetries = constants::TELEGRAM_MAX_RETRIES) {
    Error lastErr{ErrorCode::NetworkError, "no attempt"};
    for (int i = 0; i < maxRetries; ++i) {
      auto r = sendMessage(text);
      if (r) {
        ++sent_;
        last_sent_ms_.store(nowMs(), std::memory_order_relaxed);
        return r;
      }
      lastErr = r.error();
      if (r.error().code == ErrorCode::RateLimited) {
        std::int64_t waitMs = 500 * (i + 1);
        const auto& msg = r.error().message;
        auto pos = msg.find("retry_after=");
        if (pos != std::string::npos) {
          auto sec = std::strtoll(msg.c_str() + pos + 12, nullptr, 10);
          if (sec > 0) waitMs = sec * 1000;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(waitMs));
      } else if (r.error().code == ErrorCode::AuthFailed) {
        break; // no point retrying
      } else {
        std::this_thread::sleep_for(std::chrono::milliseconds(100 * (i + 1)));
      }
    }
    ++failed_;
    return lastErr;
  }

  Result<TelegramResponse> sendRequest(const TelegramRequest& treq) {
    std::string text = treq.text;
    return sendMessageWithRetry(text);
  }

  static Result<TelegramResponse> parseResponse(const HttpResponse& http) {
    TelegramResponse resp;
    resp.raw = http.body;
    if (!http.ok()) {
      resp.ok = false;
      resp.errorCode = http.status;
      resp.description = "HTTP " + std::to_string(http.status);
      if (http.status == 429) {
        std::int64_t retryAfterSec = 1;
        auto ra = http.body.find("\"retry_after\"");
        if (ra != std::string::npos) {
          auto colon = http.body.find(':', ra);
          if (colon != std::string::npos)
            retryAfterSec = std::strtoll(http.body.c_str() + colon + 1, nullptr, 10);
        }
        resp.description = std::string("rate limited retry_after=") + std::to_string(retryAfterSec);
        return Error{ErrorCode::RateLimited, resp.description};
      }
      if (http.status == 401 || http.status == 403) {
        return Error{ErrorCode::AuthFailed, resp.description};
      }
      return Error{ErrorCode::NetworkError, resp.description};
    }
    // Minimal JSON field extraction
    resp.ok = http.body.find("\"ok\":true") != std::string::npos ||
              http.body.find("\"ok\": true") != std::string::npos;
    auto mid = http.body.find("\"message_id\"");
    if (mid != std::string::npos) {
      auto colon = http.body.find(':', mid);
      if (colon != std::string::npos) {
        resp.messageId = std::strtoll(http.body.c_str() + colon + 1, nullptr, 10);
      }
    }
    if (!resp.ok) {
      resp.description = "telegram ok=false";
      return Error{ErrorCode::NetworkError, resp.description};
    }
    return resp;
  }

  std::uint64_t sentCount() const noexcept {
    return sent_.load(std::memory_order_relaxed);
  }
  std::uint64_t failedCount() const noexcept {
    return failed_.load(std::memory_order_relaxed);
  }
  TimestampMs lastSentMs() const noexcept {
    return last_sent_ms_.load(std::memory_order_relaxed);
  }
  HttpClient& http() noexcept { return *http_; }

protected:
  TelegramCredentials creds_;
  std::shared_ptr<HttpClient> http_;
  std::atomic<std::uint64_t> sent_{0}, failed_{0};
  std::atomic<TimestampMs> last_sent_ms_{0};
};

} // namespace crashcore
