#pragma once
/**
 * Minimal async-capable HTTPS client interface for Telegram Bot API.
 * Production implementation uses Boost.Beast; default is a recording stub
 * that allows offline tests and request construction validation.
 */
#include "common/result.hpp"
#include "common/errors.hpp"
#include "timing/timestamp.hpp"
#include <atomic>
#include <functional>
#include <mutex>
#include <string>
#include <vector>
#include <map>

namespace crashcore {

struct HttpRequest {
  std::string method = "POST";
  std::string host;
  std::string path;
  int port = 443;
  bool tls = true;
  std::map<std::string, std::string> headers;
  std::string body;
  std::int64_t timeout_ms = 10000;
};

struct HttpResponse {
  int status = 0;
  std::string body;
  std::map<std::string, std::string> headers;
  bool ok() const noexcept { return status >= 200 && status < 300; }
};

class HttpClient {
public:
  using ResponseHandler = std::function<void(Result<HttpResponse>)>;

  virtual ~HttpClient() = default;

  virtual Result<HttpResponse> request(const HttpRequest& req) {
    // Stub: record and return synthetic 200
    {
      std::lock_guard lk(mu_);
      history_.push_back(req);
      if (history_.size() > 256) history_.erase(history_.begin());
    }
    ++requests_;
    if (fail_next_.exchange(false)) {
      ++failures_;
      return Error{ErrorCode::NetworkError, "injected failure"};
    }
    HttpResponse resp;
    resp.status = 200;
    resp.body = "{\"ok\":true,\"result\":{\"message_id\":1}}";
    last_ms_.store(nowMs(), std::memory_order_relaxed);
    return resp;
  }

  void failNext() { fail_next_.store(true); }

  std::uint64_t requestCount() const noexcept {
    return requests_.load(std::memory_order_relaxed);
  }
  std::uint64_t failureCount() const noexcept {
    return failures_.load(std::memory_order_relaxed);
  }

  std::vector<HttpRequest> history() const {
    std::lock_guard lk(mu_);
    return history_;
  }

protected:
  mutable std::mutex mu_;
  std::vector<HttpRequest> history_;
  std::atomic<std::uint64_t> requests_{0};
  std::atomic<std::uint64_t> failures_{0};
  std::atomic<bool> fail_next_{false};
  std::atomic<TimestampMs> last_ms_{0};
};

} // namespace crashcore
