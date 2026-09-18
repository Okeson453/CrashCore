#pragma once

#include <string>

namespace pe {

struct RegimeClusterState {
  int clusterId = -1;
  double clusterDistance = 0.0;
  double clusterConfidence = 0.0;
  int regimeDuration = 0;
  double transitionProbability = 0.0;
  int sampleCount = 0;
  double historicalHitRate = 0.0;
  double historicalCalibration = 0.0;
  std::string label;
};

} // namespace pe
