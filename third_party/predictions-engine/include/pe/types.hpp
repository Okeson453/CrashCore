#pragma once

/**
 * Prediction subsystem domain types (ported from types.ts).
 * Analytics = what happened; Prediction = model estimate; Risk = allowed?; Execution = act.
 */

#include <array>
#include <cstdint>
#include <optional>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

namespace pe {

using ThresholdTarget = double; // 1.3 | 2.0 | 5.0 | 10.0
inline constexpr std::array<ThresholdTarget, 4> SUPPORTED_TARGETS = {1.3, 2.0, 5.0, 10.0};

using FeatureVersion = std::string;
using TargetVersion = std::string;

enum class FeaturePath {
  V2_INCREMENTAL,
  V1_FALLBACK,
  ACIE_STATE
};

inline const char* toString(FeaturePath p) {
  switch (p) {
    case FeaturePath::V2_INCREMENTAL: return "V2_INCREMENTAL";
    case FeaturePath::V1_FALLBACK:    return "V1_FALLBACK";
    case FeaturePath::ACIE_STATE:     return "ACIE_STATE";
  }
  return "UNKNOWN";
}

struct ModelIdentity {
  std::string name;
  std::string version;
  FeatureVersion featureVersion;
  TargetVersion targetVersion;
};

enum class DataQuality { High, Medium, Low };

struct HistoricalRound {
  std::string id;
  std::string externalRoundId;
  std::optional<std::string> sessionId;
  std::optional<std::string> startedAt;
  std::optional<std::string> crashedAt;
  double crashPoint = 0.0;
  std::optional<std::string> observationSource;
  std::optional<DataQuality> dataQuality;
  std::string createdAt;
  std::optional<int> sequenceIndex;
};

struct FeatureVector {
  std::string roundId;
  std::string timestamp;
  FeatureVersion featureVersion;
  std::unordered_map<std::string, double> values;
  struct Meta {
    int sampleSize = 0;
    double dataQualityScore = 0.0;
    int missingFeatureCount = 0;
    std::optional<std::string> regimeHint;
  } meta;
};

struct Label {
  std::string roundId;
  TargetVersion targetVersion;
  std::unordered_map<std::string, int> thresholds; // 0 | 1
  double crashPoint = 0.0;
  std::string timestamp;
};

struct DatasetRow {
  FeatureVector features;
  Label label;
};

struct DatasetMeta {
  std::string id;
  FeatureVersion featureVersion;
  TargetVersion targetVersion;
  std::string sourceFrom;
  std::string sourceTo;
  std::string generatedAt;
  int sampleCount = 0;
  std::unordered_map<std::string, double> classDistribution;
  std::unordered_map<std::string, double> missingDataStats;
  bool leakageCheckPassed = false;
  std::string configHash;
};

struct Dataset {
  DatasetMeta meta;
  std::vector<DatasetRow> rows;
};

enum class StreakState { Low, High, Mixed, Neutral };

inline const char* toString(StreakState s) {
  switch (s) {
    case StreakState::Low:     return "low";
    case StreakState::High:    return "high";
    case StreakState::Mixed:   return "mixed";
    case StreakState::Neutral: return "neutral";
  }
  return "neutral";
}

struct Regime {
  /** Deterministic regime classification key (e.g. "neutral", "deep-low"). */
  std::string id;
  std::string name;
  /** Optional per-detection UUID for tracing only — not for grouping. */
  std::optional<std::string> instanceId;
  struct Dimensions {
    double lowMultiplierConcentration = 0.0;
    double highMultiplierConcentration = 0.0;
    double volatility = 0.0;
    StreakState streakState = StreakState::Neutral;
    std::unordered_map<std::string, double> thresholdFrequency;
    bool anomalyState = false;
  } dimensions;
  double confidence = 0.0;
  std::vector<std::string> explanation;
  std::string detectedAt;
};

struct PredictionOutput {
  std::string predictionId;
  ModelIdentity model;
  ThresholdTarget target = 1.3;
  double score = 0.0;
  double probability = 0.0;
  double confidence = 0.0;
  std::optional<Regime> regime;
  double dataQuality = 0.0;
  std::unordered_map<std::string, double> featureSummary;
  std::vector<std::string> reasoning;
  std::string timestamp;
  std::string expiresAt;
};

/**
 * Canonical, immutable prediction signal.
 * Built once via toSignal(); only ever exposed as const&.
 */
struct PredictionSignal {
  std::string predictionId;
  std::string timestamp;
  std::string modelVersion;
  std::string featureVersion;
  FeaturePath featurePath = FeaturePath::V2_INCREMENTAL;
  std::string targetRoundId;
  ThresholdTarget target = 1.3;
  double score = 0.0;
  double probability = 0.0;
  double confidence = 0.0;
  std::optional<std::string> regimeId;
  double dataQuality = 0.0;
  std::vector<std::string> reasoning;
  std::string expiresAt;
  std::unordered_map<std::string, double> featureSummary;
};

struct ValidationMetrics {
  int sampleSize = 0;
  double baselineProbability = 0.0;
  double conditionalProbability = 0.0;
  double precision = 0.0;
  double recall = 0.0;
  double f1 = 0.0;
  double falsePositiveRate = 0.0;
  double falseNegativeRate = 0.0;
  double calibrationError = 0.0;
  std::array<double, 2> confidenceInterval95 = {0.0, 0.0};
  std::optional<double> brierScore;
  std::optional<double> expectedCalibrationError;
  std::optional<std::unordered_map<std::string, ValidationMetrics>> regimeBreakdown;
};

/** Pipeline stage for error tagging (C++-idiomatic enum). */
enum class PipelineStage {
  FeatureGeneration,
  RegimeDetection,
  ModelResolution,
  ModelPrediction,
  MlInference,
  Fusion,
  PredictionOutputValidation,
  SignalConversion,
  SignalValidation
};

inline const char* toString(PipelineStage s) {
  switch (s) {
    case PipelineStage::FeatureGeneration:          return "feature_generation";
    case PipelineStage::RegimeDetection:            return "regime_detection";
    case PipelineStage::ModelResolution:            return "model_resolution";
    case PipelineStage::ModelPrediction:            return "model_prediction";
    case PipelineStage::MlInference:                return "ml_inference";
    case PipelineStage::Fusion:                     return "fusion";
    case PipelineStage::PredictionOutputValidation: return "prediction_output_validation";
    case PipelineStage::SignalConversion:           return "signal_conversion";
    case PipelineStage::SignalValidation:           return "signal_validation";
  }
  return "unknown";
}

class PipelineStageError : public std::runtime_error {
public:
  PipelineStage stage;
  explicit PipelineStageError(PipelineStage st, const std::string& msg)
    : std::runtime_error(std::string(toString(st)) + ": " + msg), stage(st) {}
};

} // namespace pe
