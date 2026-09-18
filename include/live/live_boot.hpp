#pragma once
#include "observability/readiness_barrier.hpp"
#include "live/live_supervisor.hpp"
#include "live/cold_start_seeder.hpp"
#include "ingestion/rest/poll_worker.hpp"
#include "timing/timestamp.hpp"
#include <functional>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>

namespace crashcore {

class LiveBoot {
public:
  struct BootResult {
    bool ok = false;
    ReadinessStage finalStage = ReadinessStage::Booting;
    std::int64_t durationMs = 0;
    std::string error;
  };
  struct Deps {
    LiveSupervisor* supervisor = nullptr;
    ColdStartSeeder* seeder = nullptr;
    PollWorker* pollWorker = nullptr;
    ReadinessBarrier* barrier = nullptr;
    std::function<Result<void>()> validateSchema;
    std::function<void()> prewarmPrediction;
  };
  explicit LiveBoot(Deps deps) : deps_(std::move(deps)) {
    if (!deps_.barrier) {
      owned_barrier_ = std::make_unique<ReadinessBarrier>();
      deps_.barrier = owned_barrier_.get();
    }
  }
  ReadinessBarrier& barrier() noexcept { return *deps_.barrier; }

  template <typename Fn>
  auto withBootStage(const std::string& name, Fn&& fn) -> decltype(fn()) {
    const auto t0 = steadyNs();
    auto result = fn();
    stage_durations_.push_back({name, (steadyNs() - t0) / 1000000});
    return result;
  }

  BootResult run() {
    const auto t0 = nowMs();
    BootResult br;
    try {
      deps_.barrier->advanceTo(ReadinessStage::Booting);
      if (deps_.validateSchema) {
        withBootStage("validate_schema", [&] {
          auto r = deps_.validateSchema();
          if (!r) throw std::runtime_error(r.error().message);
          return 0;
        });
      }
      if (deps_.supervisor) {
        withBootStage("acquire_lease", [&] {
          auto r = deps_.supervisor->acquire();
          if (!r) throw std::runtime_error(r.error().message);
          deps_.supervisor->startRenewLoop();
          return 0;
        });
      }
      deps_.barrier->advanceTo(ReadinessStage::MinimalStateReady);
      if (deps_.seeder) {
        withBootStage("cold_start_seed", [&] {
          auto r = deps_.seeder->seed();
          if (!r) throw std::runtime_error(r.error().message);
          return 0;
        });
      }
      deps_.barrier->advanceTo(ReadinessStage::PredictionStateReady);
      if (deps_.prewarmPrediction) {
        withBootStage("prewarm_prediction", [&] { deps_.prewarmPrediction(); return 0; });
      }
      if (deps_.pollWorker) {
        withBootStage("start_poll_worker", [&] { deps_.pollWorker->start(); return 0; });
      }
      deps_.barrier->advanceTo(ReadinessStage::LiveN1Ready);
      deps_.barrier->advanceTo(ReadinessStage::FullHydrationReady);
      br.ok = true;
      br.finalStage = deps_.barrier->stage();
    } catch (const std::exception& ex) {
      br.ok = false; br.error = ex.what(); br.finalStage = deps_.barrier->stage();
    }
    br.durationMs = nowMs() - t0;
    return br;
  }
  void shutdown() {
    if (deps_.pollWorker) deps_.pollWorker->stop();
    if (deps_.supervisor) { deps_.supervisor->stopRenewLoop(); deps_.supervisor->release(); }
  }
  const std::vector<std::pair<std::string, std::int64_t>>& stageDurations() const { return stage_durations_; }
private:
  Deps deps_;
  std::unique_ptr<ReadinessBarrier> owned_barrier_;
  std::vector<std::pair<std::string, std::int64_t>> stage_durations_;
};

} // namespace crashcore
