#pragma once
#include "ingestion/rest/crash_history_client.hpp"
#include "ingestion/event_router.hpp"
#include "common/result.hpp"
#include "common/errors.hpp"
#include "timing/timestamp.hpp"
#include <atomic>
#include <memory>

namespace crashcore {
class PollRecovery {
public:
  explicit PollRecovery(std::shared_ptr<CrashHistoryClient> client = nullptr,
                        EventRouter* router = nullptr)
      : client_(client ? std::move(client)
                       : std::make_shared<CrashHistoryClient>(std::make_shared<HttpClient>()))
      , router_(router) {}
  void setRouter(EventRouter* r) { router_ = r; }
  Result<void> tick() {
    ++ticks_;
    std::size_t n = 0;
    for (int page = 1; page <= 2; ++page) {
      auto r = client_->fetchPage(page);
      if (!r) { ++errors_; return Error{r.error().code, r.error().message}; }
      for (const auto& fr : r.value()) {
        if (router_) {
          CrashEvent ev;
          ev.kind = EventKind::End; ev.gameId = fr.gameId; ev.roundId = fr.gameId;
          ev.crashPoint = fr.multiplier; ev.hash = fr.hash;
          ev.beganAtMs = fr.beganAtMs; ev.endedAtMs = fr.crashedAtMs;
          ev.eventTimeMs = fr.crashedAtMs ? fr.crashedAtMs : nowMs();
          ev.valid = true;
          router_->route(ev);
        }
        ++n; ++injected_;
      }
    }
    fetched_ += n;
    return Result<void>::success();
  }
  void injectHistory(const CrashEvent& ev) { if (router_) router_->route(ev); ++injected_; }
  std::uint64_t tickCount() const noexcept { return ticks_.load(); }
  std::uint64_t fetchedCount() const noexcept { return fetched_.load(); }
  std::uint64_t injectedCount() const noexcept { return injected_.load(); }
private:
  std::shared_ptr<CrashHistoryClient> client_;
  EventRouter* router_ = nullptr;
  std::atomic<std::uint64_t> fetched_{0}, ticks_{0}, injected_{0}, errors_{0};
};
} // namespace crashcore
