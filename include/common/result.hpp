#pragma once
#include "common/errors.hpp"
#include <utility>
#include <variant>
#include <optional>

namespace crashcore {

template <typename T>
class Result {
public:
  Result(T value) : data_(std::move(value)) {}
  Result(Error err) : data_(std::move(err)) {}
  Result(ErrorCode code) : data_(Error{code}) {}
  Result(ErrorCode code, std::string msg) : data_(Error{code, std::move(msg)}) {}

  bool ok() const noexcept { return std::holds_alternative<T>(data_); }
  explicit operator bool() const noexcept { return ok(); }

  T& value() & { return std::get<T>(data_); }
  const T& value() const& { return std::get<T>(data_); }
  T&& value() && { return std::get<T>(std::move(data_)); }

  Error& error() & { return std::get<Error>(data_); }
  const Error& error() const& { return std::get<Error>(data_); }

  T value_or(T fallback) const {
    if (ok()) return std::get<T>(data_);
    return fallback;
  }

  template <typename F>
  auto map(F&& f) const -> Result<decltype(f(std::declval<T>()))> {
    using U = decltype(f(std::declval<T>()));
    if (!ok()) return Result<U>(std::get<Error>(data_));
    return Result<U>(f(std::get<T>(data_)));
  }

private:
  std::variant<T, Error> data_;
};

template <>
class Result<void> {
public:
  Result() : err_() {}
  Result(Error err) : err_(std::move(err)) {}
  Result(ErrorCode code) : err_(Error{code}) {}
  Result(ErrorCode code, std::string msg) : err_(Error{code, std::move(msg)}) {}

  bool ok() const noexcept { return !err_; }
  explicit operator bool() const noexcept { return ok(); }
  const Error& error() const& { return err_; }
  Error& error() & { return err_; }

  static Result<void> success() { return Result<void>{}; }

private:
  Error err_;
};

} // namespace crashcore
