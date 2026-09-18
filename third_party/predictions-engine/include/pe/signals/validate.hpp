#pragma once

#include "pe/types.hpp"

#include <cmath>
#include <stdexcept>
#include <string>

namespace pe {

/** Two-stage validation of PredictionOutput before signal conversion. */
inline void validatePredictionOutput(const PredictionOutput& out) {
  if (out.predictionId.empty())
    throw PipelineStageError(PipelineStage::PredictionOutputValidation,
                             "missing predictionId");
  if (!std::isfinite(out.probability) || out.probability < 0.0 || out.probability > 1.0)
    throw PipelineStageError(PipelineStage::PredictionOutputValidation,
                             "probability out of [0,1]");
  if (!std::isfinite(out.confidence) || out.confidence < 0.0 || out.confidence > 1.0)
    throw PipelineStageError(PipelineStage::PredictionOutputValidation,
                             "confidence out of [0,1]");
  if (out.model.name.empty() || out.model.version.empty())
    throw PipelineStageError(PipelineStage::PredictionOutputValidation,
                             "model identity incomplete");
}

/** Validate the final signal. */
inline void validateSignal(const PredictionSignal& s) {
  if (s.predictionId.empty())
    throw PipelineStageError(PipelineStage::SignalValidation, "missing predictionId");
  if (s.targetRoundId.empty())
    throw PipelineStageError(PipelineStage::SignalValidation, "missing targetRoundId");
  if (!std::isfinite(s.probability) || s.probability < 0.0 || s.probability > 1.0)
    throw PipelineStageError(PipelineStage::SignalValidation, "probability out of range");
  if (s.featureVersion.empty())
    throw PipelineStageError(PipelineStage::SignalValidation, "missing featureVersion");
}

} // namespace pe
