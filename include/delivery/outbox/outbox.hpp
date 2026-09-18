#pragma once
#include "common/constants.hpp"
/**
 * Durable outbox for signals/notifications.
 * Async only — never on the prediction hot path.
 * Behaviour informed by TestingEngine outbox-* tests and migrations.
 */
#include "delivery/outbox/outbox_queue.hpp"
#include "delivery/signal.hpp"
#include "timing/timestamp.hpp"
#include "common/result.hpp"
#include <unordered_set>
#include <mutex>
#include <string>
#include <atomic>

namespace crashcore {

class Outbox {
public:
  explicit Outbox(std::size_t capacity = 2048) : queue_(capacity) {}

  Result<std::uint64_t> publish(Signal signal) {
    // Exactly-once per dedupe key (prediction id + kind)
    const std::string key = signal.predictionId + ":" +
        std::to_string(static_cast<int>(signal.kind));
    {
      std::lock_guard lk(dedupe_mu_);
      if (delivered_keys_.count(key) || pending_keys_.count(key)) {
        ++duplicates_;
        return Error{ErrorCode::Duplicate, "already published"};
      }
      // Reserve key as pending
      pending_keys_.insert(key);
    }

    OutboxItem item;
    item.state = OutboxState::Pending;
    item.signal = std::move(signal);
    item.createdAtMs = nowMs();
    item.dedupeKey = key;
    if (!queue_.enqueue(std::move(item))) {
      std::lock_guard lk(dedupe_mu_);
      pending_keys_.erase(key);
      return Error{ErrorCode::QueueFull, "outbox full"};
    }
    ++published_;
    return queue_.enqueuedCount(); // approximate id
  }

  std::optional<OutboxItem> claim(std::chrono::milliseconds timeout = std::chrono::milliseconds(100)) {
    auto item = queue_.dequeue(timeout);
    if (!item) return std::nullopt;
    item->state = OutboxState::Claimed;
    item->claimedAtMs = nowMs();
    ++claimed_;
    return item;
  }

  void markDelivered(OutboxItem& item) {
    item.state = OutboxState::Delivered;
    item.deliveredAtMs = nowMs();
    std::lock_guard lk(dedupe_mu_);
    pending_keys_.erase(item.dedupeKey);
    delivered_keys_.insert(item.dedupeKey);
    if (delivered_keys_.size() > 50'000) delivered_keys_.clear();
    ++delivered_;
  }

  void markFailed(OutboxItem& item, std::string error) {
    item.lastError = std::move(error);
    item.attempts += 1;
    if (item.attempts >= constants::OUTBOX_MAX_ATTEMPTS) {
      item.state = OutboxState::DeadLetter;
      ++dead_letters_;
      std::lock_guard lk(dedupe_mu_);
      pending_keys_.erase(item.dedupeKey);
    } else {
      item.state = OutboxState::Pending;
      // re-queue
      queue_.enqueue(std::move(item));
    }
    ++failed_;
  }

  struct Stats {
    std::uint64_t published = 0, claimed = 0, delivered = 0, failed = 0;
    std::uint64_t duplicates = 0, deadLetters = 0;
  };
  Stats stats() const {
    return {published_, claimed_, delivered_, failed_, duplicates_, dead_letters_};
  }

  std::size_t pendingSize() const { return queue_.size(); }

private:
  OutboxQueue queue_;
  std::mutex dedupe_mu_;
  std::unordered_set<std::string> pending_keys_;
  std::unordered_set<std::string> delivered_keys_;
  std::atomic<std::uint64_t> published_{0}, claimed_{0}, delivered_{0};
  std::atomic<std::uint64_t> failed_{0}, duplicates_{0}, dead_letters_{0};
};

} // namespace crashcore
