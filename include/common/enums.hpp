#pragma once
#include <cstdint>
#include <string_view>

namespace crashcore {

enum class LogLevel : std::uint8_t {
  Trace = 0,
  Debug,
  Info,
  Warn,
  Error,
  Fatal
};

enum class ComponentId : std::uint16_t {
  Application = 0,
  Orchestrator,
  Ingestion,
  WebSocket,
  SocketIo,
  Signing,
  Heartbeat,
  EventDecoder,
  EventRouter,
  PredictionInterface,
  Validation,
  OutcomeMatcher,
  PredictionRegistry,
  Feedback,
  Outbox,
  Telegram,
  Persistence,
  WorkerPool,
  Telemetry,
  Logging,
  Security,
  COUNT
};

inline std::string_view toString(LogLevel l) noexcept {
  switch (l) {
    case LogLevel::Trace: return "TRACE";
    case LogLevel::Debug: return "DEBUG";
    case LogLevel::Info:  return "INFO";
    case LogLevel::Warn:  return "WARN";
    case LogLevel::Error: return "ERROR";
    case LogLevel::Fatal: return "FATAL";
  }
  return "UNKNOWN";
}

inline std::string_view toString(ComponentId c) noexcept {
  switch (c) {
    case ComponentId::Application:         return "application";
    case ComponentId::Orchestrator:        return "orchestrator";
    case ComponentId::Ingestion:           return "ingestion";
    case ComponentId::WebSocket:           return "websocket";
    case ComponentId::SocketIo:            return "socketio";
    case ComponentId::Signing:             return "signing";
    case ComponentId::Heartbeat:           return "heartbeat";
    case ComponentId::EventDecoder:        return "event_decoder";
    case ComponentId::EventRouter:         return "event_router";
    case ComponentId::PredictionInterface: return "prediction_interface";
    case ComponentId::Validation:          return "validation";
    case ComponentId::OutcomeMatcher:      return "outcome_matcher";
    case ComponentId::PredictionRegistry:  return "prediction_registry";
    case ComponentId::Feedback:            return "feedback";
    case ComponentId::Outbox:              return "outbox";
    case ComponentId::Telegram:            return "telegram";
    case ComponentId::Persistence:         return "persistence";
    case ComponentId::WorkerPool:          return "worker_pool";
    case ComponentId::Telemetry:           return "telemetry";
    case ComponentId::Logging:             return "logging";
    case ComponentId::Security:            return "security";
    default:                               return "unknown";
  }
}

} // namespace crashcore
