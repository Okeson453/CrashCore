#pragma once
#include "delivery/signal.hpp"
#include "delivery/telegram/message_format.hpp"
#include "validation/outcome.hpp"
#include "prediction_interface/prediction_event.hpp"
#include <string>
#include <sstream>

namespace crashcore {

class SignalFormatter {
public:
  static std::string formatTelegram(const Signal& s) {
    std::ostringstream oss;
    switch (s.kind) {
      case SignalKind::Prediction:
        oss << "🎯 " << s.text;
        break;
      case SignalKind::Outcome:
        oss << "📊 " << s.text;
        break;
      case SignalKind::Heartbeat:
        oss << "💓 " << s.text;
        break;
      case SignalKind::SystemAlert:
        oss << "⚠️ " << s.text;
        break;
    }
    if (!s.correlationId.empty()) {
      oss << "\n[" << s.correlationId << "]";
    }
    return oss.str();
  }

  static std::string formatPlain(const Signal& s) { return s.text; }

  static std::string formatPrediction(const PredictionEvent& e) {
    return formatPredictionMessage(e);
  }

  static std::string formatOutcome(const Outcome& o) {
    return formatValidationMessage(o);
  }
};

} // namespace crashcore
