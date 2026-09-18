#pragma once
#include <vector>
#include <mutex>
#include <memory>
#include <functional>
#include <cstddef>

namespace crashcore {

/**
 * Simple object pool to reduce allocation on semi-hot paths.
 * Not lock-free; use only off the absolute critical path or with care.
 */
template <typename T>
class ObjectPool {
public:
  explicit ObjectPool(std::size_t prealloc = 32, std::size_t max = 1024)
      : max_(max) {
    pool_.reserve(prealloc);
    for (std::size_t i = 0; i < prealloc; ++i)
      pool_.push_back(std::make_unique<T>());
  }

  std::unique_ptr<T> acquire() {
    std::lock_guard lk(mu_);
    if (!pool_.empty()) {
      auto p = std::move(pool_.back());
      pool_.pop_back();
      ++acquired_;
      return p;
    }
    ++created_;
    return std::make_unique<T>();
  }

  void release(std::unique_ptr<T> obj) {
    if (!obj) return;
    std::lock_guard lk(mu_);
    if (pool_.size() < max_) {
      // Reset if T has a clear/reset method is caller's responsibility
      pool_.push_back(std::move(obj));
      ++released_;
    }
  }

  std::size_t available() const {
    std::lock_guard lk(mu_);
    return pool_.size();
  }

  std::uint64_t acquired_count() const { std::lock_guard lk(mu_); return acquired_; }
  std::uint64_t created_count() const { std::lock_guard lk(mu_); return created_; }

private:
  std::size_t max_;
  std::vector<std::unique_ptr<T>> pool_;
  mutable std::mutex mu_;
  std::uint64_t acquired_ = 0, released_ = 0, released_ = 0;
};

} // namespace crashcore
