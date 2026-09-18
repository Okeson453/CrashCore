#pragma once
#include <functional>
#include <utility>

namespace crashcore {

/** Move-only callable task for worker queues. */
class Task {
public:
  Task() = default;

  template <typename F>
  explicit Task(F&& f) : fn_(std::forward<F>(f)) {}

  Task(Task&&) noexcept = default;
  Task& operator=(Task&&) noexcept = default;
  Task(const Task&) = delete;
  Task& operator=(const Task&) = delete;

  explicit operator bool() const noexcept { return static_cast<bool>(fn_); }

  void operator()() {
    if (fn_) fn_();
  }

  void reset() { fn_ = nullptr; }

private:
  std::function<void()> fn_;
};

} // namespace crashcore
