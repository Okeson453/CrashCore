#pragma once
#include <optional>
/**
 * PORT_BEHAVIOR from TestingEngine notifications/telegram.ts
 * formatPredictionMessage / formatValidationMessage
 */
#include "common/types.hpp"
#include "validation/outcome.hpp"
#include "prediction_interface/prediction_event.hpp"
#include <sstream>
#include <iomanip>
#include <string>
#include <cmath>

namespace crashcore {

inline std::string fmtMult(double m) {
  std::ostringstream oss;
  oss << std::fixed << std::setprecision(2) << m << "x";
  return oss.str();
}

inline std::string fmtProb(double p) {
  std::ostringstream oss;
  oss << std::fixed << std::setprecision(1) << (p * 100.0) << "%";
  return oss.str();
}

inline std::string fmtConfidence(double c) {
  std::ostringstream oss;
  oss << std::fixed << std::setprecision(2) << c;
  return oss.str();
}

struct PredictionForMessage {
  std::string predictionId;
  double targetMultiplier = 0;
  double probability = 0;
  double confidence = 0;
  std::string regimeName;
  std::optional<double> lastRoundMultiplier;
  std::string generatedAt;
};

struct ValidationForMessage {
  std::string predictionId;
  std::string gameId;
  double targetMultiplier = 0;
  double actualMultiplier = 0;
  double probability = 0;
  std::string result; // WIN | LOSS
  std::string resolvedAt;
};

inline PredictionForMessage fromPredictionEvent(const PredictionEvent& e) {
  PredictionForMessage m;
  m.predictionId = e.predictionId;
  m.targetMultiplier = e.targetMult;
  m.probability = e.confidence; // contract: confidence used as display prob boundary
  m.confidence = e.confidence;
  m.generatedAt = std::to_string(e.createdAtMs);
  return m;
}

inline ValidationForMessage fromOutcome(const Outcome& o) {
  ValidationForMessage m;
  m.predictionId = o.predictionId;
  m.gameId = o.roundId;
  m.targetMultiplier = o.targetMult;
  m.actualMultiplier = o.actualMult;
  m.result = o.isWin ? "WIN" : "LOSS";
  if (o.result == PredictionOutcome::Void) m.result = "VOID";
  if (o.result == PredictionOutcome::Stale) m.result = "STALE";
  m.resolvedAt = std::to_string(o.resolvedAtMs);
  return m;
}

inline std::string formatPredictionMessage(const PredictionForMessage& p) {
  std::ostringstream oss;
  oss << "🎯 PREDICTION\n"
      << "Target: " << fmtMult(p.targetMultiplier) << "\n"
      << "Prob: " << fmtProb(p.probability) << "\n"
      << "Confidence: " << fmtConfidence(p.confidence) << "\n"
      << "Regime: " << (p.regimeName.empty() ? "n/a" : p.regimeName);
  if (p.lastRoundMultiplier)
    oss << "\nLast: " << fmtMult(*p.lastRoundMultiplier);
  return oss.str();
}

inline std::string formatValidationMessage(const ValidationForMessage& v) {
  std::string icon = v.result == "WIN" ? "✅ WIN" : (v.result == "LOSS" ? "❌ LOSS" : v.result);
  std::ostringstream oss;
  oss << icon << " @ " << fmtMult(v.actualMultiplier) << "\n"
      << "Target: " << fmtMult(v.targetMultiplier) << "\n"
      << "Game: " << v.gameId;
  return oss.str();
}

inline std::string formatPredictionMessage(const PredictionEvent& e) {
  return formatPredictionMessage(fromPredictionEvent(e));
}

inline std::string formatValidationMessage(const Outcome& o) {
  return formatValidationMessage(fromOutcome(o));
}

} // namespace crashcore
