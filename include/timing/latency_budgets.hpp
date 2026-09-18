#pragma once
/**
 * PORT_CONTRACT from TestingEngine live/latency-budgets.ts + constants
 */
#include "common/constants.hpp"
#include "timing/latency_tracker.hpp"
#include <string>
#include <vector>
#include <sstream>

namespace crashcore {

struct BudgetCheck {
  LatencyTracker::Stage stage;
  std::int64_t budgetUs;
  std::int64_t observedAvgUs;
  std::int64_t observedMaxUs;
  bool ok;
};

class LatencyBudgetMonitor {
public:
  explicit LatencyBudgetMonitor(LatencyTracker& tracker) : tracker_(tracker) {}

  std::vector<BudgetCheck> evaluate() const {
    std::vector<BudgetCheck> out;
    auto add = [&](LatencyTracker::Stage s, std::int64_t budget) {
      auto st = tracker_.stats(s);
      BudgetCheck c;
      c.stage = s;
      c.budgetUs = budget;
      c.observedAvgUs = static_cast<std::int64_t>(st.avgUs);
      c.observedMaxUs = st.maxUs;
      c.ok = st.count == 0 || st.avgUs <= static_cast<double>(budget);
      out.push_back(c);
    };
    add(LatencyTracker::Stage::Parse, constants::BUDGET_PARSE_US);
    add(LatencyTracker::Stage::State, constants::BUDGET_STATE_US);
    add(LatencyTracker::Stage::Ipc, constants::BUDGET_IPC_US);
    add(LatencyTracker::Stage::Queue, constants::BUDGET_QUEUE_US);
    add(LatencyTracker::Stage::Prediction, constants::BUDGET_PREDICTION_US);
    add(LatencyTracker::Stage::Validation, constants::BUDGET_VALIDATION_US);
    add(LatencyTracker::Stage::Outbox, constants::BUDGET_OUTBOX_US);
    add(LatencyTracker::Stage::Total, constants::BUDGET_TOTAL_APP_US);
    return out;
  }

  bool allOk() const {
    for (const auto& c : evaluate()) if (!c.ok) return false;
    return true;
  }

  std::string report() const {
    std::ostringstream oss;
    for (const auto& c : evaluate()) {
      oss << "stage=" << static_cast<int>(c.stage)
          << " budget_us=" << c.budgetUs
          << " avg_us=" << c.observedAvgUs
          << " max_us=" << c.observedMaxUs
          << " ok=" << (c.ok ? "1" : "0") << "\n";
    }
    return oss.str();
  }

private:
  LatencyTracker& tracker_;
};

} // namespace crashcore
