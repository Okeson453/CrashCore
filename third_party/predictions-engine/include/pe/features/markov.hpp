#pragma once

#include "pe/state/incremental_state_engine.hpp"

#include <string>
#include <unordered_map>

namespace pe {

inline std::unordered_map<std::string, double> computeMarkovFeatures(
    const IncrementalStateEngine& engine) {
  const auto& m = engine.markov();
  double pUp = engine.markovPNextAbove13();
  double pStayUp = 0.0, pStayDown = 0.0;
  const int totalUp = m.trans[1][0] + m.trans[1][1];
  const int totalDown = m.trans[0][0] + m.trans[0][1];
  if (totalUp >= 5) pStayUp = static_cast<double>(m.trans[1][1]) / totalUp;
  if (totalDown >= 5) pStayDown = static_cast<double>(m.trans[0][0]) / totalDown;
  return {
    {"markov_p_up", pUp},
    {"markov_p_stay_up", pStayUp},
    {"markov_p_stay_down", pStayDown},
  };
}

} // namespace pe
