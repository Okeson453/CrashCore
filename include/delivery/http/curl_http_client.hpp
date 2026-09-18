#pragma once
/**
 * Production HTTPS client backed by libcurl.
 * Powers Telegram delivery and BC.Game REST backfill.
 */
#include "delivery/telegram/http_client.hpp"
#include <curl/curl.h>
#include <sstream>
#include <cstring>

namespace crashcore {

namespace detail {
inline size_t curlWriteCb(char* ptr, size_t size, size_t nmemb, void* userdata) {
  auto* out = static_cast<std::string*>(userdata);
  out->append(ptr, size * nmemb);
  return size * nmemb;
}

inline size_t curlHeaderCb(char* buffer, size_t size, size_t nitems, void* userdata) {
  auto* headers = static_cast<std::map<std::string, std::string>*>(userdata);
  const size_t total = size * nitems;
  std::string line(buffer, total);
  auto colon = line.find(':');
  if (colon != std::string::npos) {
    std::string key = line.substr(0, colon);
    std::string val = line.substr(colon + 1);
    while (!val.empty() && (val.front() == ' ' || val.front() == '\t')) val.erase(val.begin());
    while (!val.empty() && (val.back() == '\r' || val.back() == '\n')) val.pop_back();
    (*headers)[key] = val;
  }
  return total;
}
} // namespace detail

class CurlHttpClient : public HttpClient {
public:
  CurlHttpClient() {
    static std::once_flag once;
    std::call_once(once, [] { curl_global_init(CURL_GLOBAL_DEFAULT); });
  }

  ~CurlHttpClient() override = default;

  Result<HttpResponse> request(const HttpRequest& req) override {
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

    CURL* curl = curl_easy_init();
    if (!curl) {
      ++failures_;
      return Error{ErrorCode::NetworkError, "curl_easy_init failed"};
    }

    std::string url;
    if (req.tls) {
      url = "https://" + req.host;
      if (req.port != 443) url += ":" + std::to_string(req.port);
    } else {
      url = "http://" + req.host;
      if (req.port != 80) url += ":" + std::to_string(req.port);
    }
    url += req.path;

    std::string responseBody;
    std::map<std::string, std::string> responseHeaders;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, detail::curlWriteCb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseBody);
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, detail::curlHeaderCb);
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, &responseHeaders);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, static_cast<long>(req.timeout_ms));
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT_MS, 5000L);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "CrashCore/1.0");
    curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L);

    struct curl_slist* hdrs = nullptr;
    for (const auto& [k, v] : req.headers) {
      std::string line = k + ": " + v;
      hdrs = curl_slist_append(hdrs, line.c_str());
    }
    if (!req.body.empty() && req.headers.find("Content-Type") == req.headers.end()) {
      hdrs = curl_slist_append(hdrs, "Content-Type: application/json");
    }
    if (hdrs) curl_easy_setopt(curl, CURLOPT_HTTPHEADER, hdrs);

    if (req.method == "POST") {
      curl_easy_setopt(curl, CURLOPT_POST, 1L);
      curl_easy_setopt(curl, CURLOPT_POSTFIELDS, req.body.c_str());
      curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, static_cast<long>(req.body.size()));
    } else if (req.method == "GET") {
      curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
    } else {
      curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, req.method.c_str());
      if (!req.body.empty()) {
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, req.body.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, static_cast<long>(req.body.size()));
      }
    }

    const CURLcode rc = curl_easy_perform(curl);
    long status = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status);

    if (hdrs) curl_slist_free_all(hdrs);
    curl_easy_cleanup(curl);

    if (rc != CURLE_OK) {
      ++failures_;
      return Error{ErrorCode::NetworkError, std::string("curl: ") + curl_easy_strerror(rc)};
    }

    HttpResponse resp;
    resp.status = static_cast<int>(status);
    resp.body = std::move(responseBody);
    resp.headers = std::move(responseHeaders);
    last_ms_.store(nowMs(), std::memory_order_relaxed);

    if (!resp.ok()) {
      ++failures_;
    }
    return resp;
  }
};

/** Factory: prefer Curl when available; fall back to recording stub. */
inline std::unique_ptr<HttpClient> makeHttpClient(bool useReal = true) {
  if (useReal) return std::make_unique<CurlHttpClient>();
  return std::make_unique<HttpClient>();
}

} // namespace crashcore
