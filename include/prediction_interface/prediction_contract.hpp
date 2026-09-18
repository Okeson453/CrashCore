#pragma once
/**
 * External Prediction Engine contract.
 * CrashCore never implements models/ACIE/features — only this boundary.
 * Inspected from TestingEngine prediction/types.ts and prediction-artifact.ts.
 */
#include "common/types.hpp"
#include "common/result.hpp"
#include <functional>
#include <string>

namespace crashcore {

struct PredictionContract {
  /** Version string the external engine is expected to speak. */
  std::string interfaceVersion = "1.0";

  /** Max time to wait for a response before treating as skip (ms). */
  std::int64_t timeoutMs = 50;

  /** Whether shared-memory SPSC is preferred over socket/API. */
  bool preferSharedMemory = true;
};

using PredictionCallback = std::function<void(Result<PredictionResponse>)>;

} // namespace crashcore
