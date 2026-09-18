#pragma once
#include <string>
#include <system_error>
#include <variant>

namespace crashcore {

enum class ErrorCode : int {
  Ok = 0,
  InvalidArgument,
  NotFound,
  AlreadyExists,
  Timeout,
  Cancelled,
  ConnectionFailed,
  ProtocolError,
  ParseError,
  SignatureError,
  AuthFailed,
  QueueFull,
  QueueEmpty,
  DatabaseError,
  NetworkError,
  RateLimited,
  InternalError,
  NotImplemented,
  Shutdown,
  StaleData,
  Duplicate,
  ValidationFailed,
  ConfigError,
  ResourceExhausted,
  IoError
};

inline const char* errorMessage(ErrorCode c) noexcept {
  switch (c) {
    case ErrorCode::Ok:                 return "ok";
    case ErrorCode::InvalidArgument:    return "invalid argument";
    case ErrorCode::NotFound:           return "not found";
    case ErrorCode::AlreadyExists:      return "already exists";
    case ErrorCode::Timeout:            return "timeout";
    case ErrorCode::Cancelled:          return "cancelled";
    case ErrorCode::ConnectionFailed:   return "connection failed";
    case ErrorCode::ProtocolError:      return "protocol error";
    case ErrorCode::ParseError:         return "parse error";
    case ErrorCode::SignatureError:     return "signature error";
    case ErrorCode::AuthFailed:         return "auth failed";
    case ErrorCode::QueueFull:          return "queue full";
    case ErrorCode::QueueEmpty:         return "queue empty";
    case ErrorCode::DatabaseError:      return "database error";
    case ErrorCode::NetworkError:       return "network error";
    case ErrorCode::RateLimited:        return "rate limited";
    case ErrorCode::InternalError:      return "internal error";
    case ErrorCode::NotImplemented:     return "not implemented";
    case ErrorCode::Shutdown:           return "shutdown";
    case ErrorCode::StaleData:          return "stale data";
    case ErrorCode::Duplicate:          return "duplicate";
    case ErrorCode::ValidationFailed:   return "validation failed";
    case ErrorCode::ConfigError:        return "config error";
    case ErrorCode::ResourceExhausted:  return "resource exhausted";
    case ErrorCode::IoError:            return "io error";
  }
  return "unknown error";
}

struct Error {
  ErrorCode   code = ErrorCode::Ok;
  std::string message;
  std::string context;

  Error() = default;
  Error(ErrorCode c) : code(c), message(errorMessage(c)) {}
  Error(ErrorCode c, std::string msg) : code(c), message(std::move(msg)) {}
  Error(ErrorCode c, std::string msg, std::string ctx)
      : code(c), message(std::move(msg)), context(std::move(ctx)) {}

  explicit operator bool() const noexcept { return code != ErrorCode::Ok; }
  bool ok() const noexcept { return code == ErrorCode::Ok; }
};

} // namespace crashcore
