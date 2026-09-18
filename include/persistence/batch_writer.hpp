#pragma once
/**
 * Async batch writer for crash rounds / predictions / outcomes.
 * Off the prediction hot path. Offline mode records without libpq.
 */
#include "persistence/database.hpp"
#include "persistence/sql_builder.hpp"
#include "common/types.hpp"
#include "validation/outcome.hpp"
#include "timing/timestamp.hpp"
#include "common/result.hpp"
#include "concurrency/bounded_queue.hpp"
#include "concurrency/task.hpp"
#include <atomic>
#include <thread>
#include <vector>
#include <mutex>
#include <string>

namespace crashcore {

struct PersistItem {
  enum class Kind : std::uint8_t { Round, Prediction, Outcome, Outbox } kind = Kind::Round;
  CrashRound round;
  PredictionEvent prediction;
  Outcome outcome;
  // optional outbox snapshot
  std::string outboxSql;
};

class BatchWriter {
public:
  explicit BatchWriter(Database* db = nullptr, std::size_t batchSize = 32,
                       std::size_t queueCapacity = 2048)
      : db_(db), batch_size_(batchSize), queue_(queueCapacity) {}

  ~BatchWriter() { stop(); }

  void start() {
    if (running_.exchange(true)) return;
    thread_ = std::jthread([this](std::stop_token st) { run(st); });
  }

  void stop() {
    running_.store(false);
    queue_.close();
    if (thread_.joinable()) {
      thread_.request_stop();
      thread_.join();
    }
    flush();
  }

  Result<void> enqueue(PersistItem item) {
    if (!queue_.try_push(std::move(item))) {
      ++drops_;
      return Error{ErrorCode::QueueFull, "persist queue full"};
    }
    ++enqueued_;
    return Result<void>::success();
  }

  Result<void> enqueueRound(const CrashRound& r) {
    PersistItem item;
    item.kind = PersistItem::Kind::Round;
    item.round = r;
    return enqueue(std::move(item));
  }

  Result<void> enqueuePrediction(const PredictionEvent& e) {
    PersistItem item;
    item.kind = PersistItem::Kind::Prediction;
    item.prediction = e;
    return enqueue(std::move(item));
  }

  Result<void> enqueueOutcome(const Outcome& o) {
    PersistItem item;
    item.kind = PersistItem::Kind::Outcome;
    item.outcome = o;
    return enqueue(std::move(item));
  }

  std::size_t flush() {
    std::vector<PersistItem> batch;
    {
      std::lock_guard lk(buf_mu_);
      batch.swap(buffer_);
    }
    // Drain queue remainder
    while (auto item = queue_.try_pop()) {
      batch.push_back(std::move(*item));
    }
    return writeBatch(batch);
  }

  std::uint64_t written() const noexcept { return written_.load(); }
  std::uint64_t offlineDrops() const noexcept { return offline_drops_.load(); }
  std::size_t offlineLogSize() const { std::lock_guard lk(offline_mu_); return offline_log_.size(); }
  std::uint64_t enqueued() const noexcept { return enqueued_.load(); }
  std::uint64_t dropped() const noexcept { return drops_.load(); }
  std::uint64_t errors() const noexcept { return errors_.load(); }

private:
  void run(std::stop_token st) {
    while (!st.stop_requested() && running_.load()) {
      auto item = queue_.pop(std::chrono::milliseconds(50));
      if (!item) {
        std::size_t n = 0;
        {
          std::lock_guard lk(buf_mu_);
          n = buffer_.size();
        }
        if (n >= batch_size_) flush();
        continue;
      }
      std::vector<PersistItem> to_write;
      {
        std::lock_guard lk(buf_mu_);
        buffer_.push_back(std::move(*item));
        if (buffer_.size() >= batch_size_) {
          to_write.swap(buffer_);
        }
      }
      if (!to_write.empty()) writeBatch(to_write);
    }
  }

  std::size_t writeBatch(const std::vector<PersistItem>& batch) {
    if (batch.empty()) return 0;
    std::size_t n = 0;
    for (const auto& item : batch) {
      std::string sql;
      switch (item.kind) {
        case PersistItem::Kind::Round:
          sql = SqlBuilder::upsertCrashRound(item.round);
          break;
        case PersistItem::Kind::Prediction:
          sql = SqlBuilder::upsertPrediction(item.prediction);
          break;
        case PersistItem::Kind::Outcome:
          sql = SqlBuilder::upsertOutcome(item.outcome);
          break;
        case PersistItem::Kind::Outbox:
          sql = item.outboxSql;
          break;
      }
      if (db_ && db_->connected()) {
        auto r = db_->execute(sql);
        if (!r) {
          ++errors_;
          continue;
        }
      } else {
        // Phase 0.11: offline drops tracked separately from written_
        std::lock_guard lk(offline_mu_);
        offline_log_.push_back(sql);
        if (offline_log_.size() > 1000) offline_log_.erase(offline_log_.begin());
        ++offline_drops_;
        continue;
      }
      ++n;
      ++written_;
    }
    return n;
  }

  Database* db_;
  std::size_t batch_size_;
  BoundedQueue<PersistItem> queue_;
  std::mutex buf_mu_;
  std::vector<PersistItem> buffer_;
  mutable std::mutex offline_mu_;
  std::vector<std::string> offline_log_;
  std::jthread thread_;
  std::atomic<bool> running_{false};
  std::atomic<std::uint64_t> written_{0}, enqueued_{0}, drops_{0}, errors_{0}, offline_drops_{0};
};

} // namespace crashcore
