#pragma once
#include "delivery/telegram/http_client.hpp"
#include "delivery/http/curl_http_client.hpp"
#include "observability/recent_round_cache.hpp"
#include "common/result.hpp"
#include "common/errors.hpp"
#include "common/types.hpp"
#include "timing/timestamp.hpp"
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include <atomic>
#include <cstdlib>

namespace crashcore {

struct FetchedRound {
  std::string gameId;
  double multiplier = 0.0;
  std::string hash;
  std::string salt;
  TimestampMs beganAtMs = 0;
  TimestampMs crashedAtMs = 0;
  bool valid() const {
    return !gameId.empty() && multiplier >= 1.0 && multiplier <= 1e6 && crashedAtMs > 0;
  }
};

inline std::optional<FetchedRound> parseFetchedRound(const std::string& gameId,
                                                     const std::string& gameDetailJson) {
  if (gameId.empty()) return std::nullopt;
  for (char c : gameId) if (c < '0' || c > '9') return std::nullopt;
  if (gameDetailJson.empty()) return std::nullopt;
  auto findNum = [&](const char* key) -> std::optional<double> {
    std::string needle = std::string("\"") + key + "\":";
    auto pos = gameDetailJson.find(needle);
    if (pos == std::string::npos) return std::nullopt;
    pos += needle.size();
    while (pos < gameDetailJson.size() && (gameDetailJson[pos] == ' ' || gameDetailJson[pos] == '"')) ++pos;
    char* end = nullptr;
    double v = std::strtod(gameDetailJson.c_str() + pos, &end);
    if (end == gameDetailJson.c_str() + pos) return std::nullopt;
    return v;
  };
  auto findStr = [&](const char* key) -> std::string {
    std::string needle = std::string("\"") + key + "\":\"";
    auto pos = gameDetailJson.find(needle);
    if (pos == std::string::npos) return {};
    pos += needle.size();
    auto end = gameDetailJson.find('"', pos);
    if (end == std::string::npos) return {};
    return gameDetailJson.substr(pos, end - pos);
  };
  auto rate = findNum("rate");
  if (!rate || *rate < 1.0 || *rate > 1e6) return std::nullopt;
  auto toMs = [](std::optional<double> v) -> TimestampMs {
    if (!v || *v < 1e12) return 0;
    return static_cast<TimestampMs>(*v);
  };
  TimestampMs crashed = toMs(findNum("endTime"));
  if (!crashed) crashed = toMs(findNum("beginTime"));
  if (!crashed) crashed = toMs(findNum("prepareTime"));
  if (!crashed) return std::nullopt;
  FetchedRound r;
  r.gameId = gameId; r.multiplier = *rate; r.hash = findStr("hash"); r.salt = findStr("salt");
  r.beganAtMs = toMs(findNum("beginTime")); r.crashedAtMs = crashed;
  return r;
}

class CrashHistoryClient {
public:
  explicit CrashHistoryClient(std::shared_ptr<HttpClient> http = nullptr)
      : http_(http ? std::move(http) : std::shared_ptr<HttpClient>(makeHttpClient(true))) {}

  Result<std::vector<FetchedRound>> fetchPage(int page, int pageSize = 50) {
    if (page < 1) page = 1;
    HttpRequest req;
    req.method = "POST"; req.host = "bc.game"; req.path = "/api/game/bet/multi/history";
    req.port = 443; req.tls = true; req.timeout_ms = 2500;
    req.headers["Content-Type"] = "application/json";
    req.headers["User-Agent"] = "Mozilla/5.0 (compatible; CrashCore/1.0)";
    req.headers["Origin"] = "https://bc.game";
    req.body = "{\"gameName\":\"crash\",\"page\":" + std::to_string(page)
             + ",\"pageSize\":" + std::to_string(pageSize) + "}";
    auto resp = http_->request(req);
    if (!resp) { ++failures_; return Error{resp.error().code, resp.error().message}; }
    if (!resp.value().ok()) { ++failures_; return Error{ErrorCode::NetworkError, "history HTTP error"}; }
    ++successes_;
    return parseHistoryBody(resp.value().body);
  }

  std::uint64_t successCount() const noexcept { return successes_.load(); }
  std::uint64_t failureCount() const noexcept { return failures_.load(); }

private:
  Result<std::vector<FetchedRound>> parseHistoryBody(const std::string& body) {
    std::vector<FetchedRound> out;
    std::size_t pos = 0;
    while (pos < body.size()) {
      auto gidPos = body.find("\"gameId\"", pos);
      if (gidPos == std::string::npos) break;
      auto colon = body.find(':', gidPos);
      auto q1 = body.find('"', colon + 1);
      auto q2 = body.find('"', q1 + 1);
      if (q1 == std::string::npos || q2 == std::string::npos) break;
      std::string gameId = body.substr(q1 + 1, q2 - q1 - 1);
      auto detailPos = body.find("\"gameDetail\"", q2);
      if (detailPos == std::string::npos || detailPos > q2 + 500) { pos = q2 + 1; continue; }
      auto dcolon = body.find(':', detailPos);
      auto dq1 = body.find('"', dcolon + 1);
      std::string detail;
      if (dq1 != std::string::npos) {
        std::size_t i = dq1 + 1;
        while (i < body.size()) {
          if (body[i] == '\\' && i + 1 < body.size()) { detail.push_back(body[i+1]); i += 2; continue; }
          if (body[i] == '"') break;
          detail.push_back(body[i]); ++i;
        }
        pos = i + 1;
      } else { pos = q2 + 1; continue; }
      auto parsed = parseFetchedRound(gameId, detail);
      if (parsed && parsed->valid()) {
        if (!globalRecentRoundCache().contains(parsed->gameId)) {
          out.push_back(*parsed);
          globalRecentRoundCache().put(CachedRound{.gameId=parsed->gameId,.multiplier=parsed->multiplier,.endedAtMs=parsed->crashedAtMs,.hash=parsed->hash});
        }
      }
    }
    return out;
  }
  std::shared_ptr<HttpClient> http_;
  std::atomic<std::uint64_t> successes_{0}, failures_{0};
};

} // namespace crashcore
