#pragma once
/** Ensemble orchestrator — weights model outputs (TE ensemble parity, subset). */
#include <algorithm>
#include <cmath>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>
namespace crashcore {
struct ModelVote {
  std::string model;
  double probability = 0;
  double confidence = 0;
  double weight = 1.0;
};
class EnsembleOrchestrator {
public:
  void setWeight(const std::string& model, double w) {
    std::lock_guard lk(mu_); weights_[model] = std::max(0.0, w);
  }
  double weight(const std::string& model) const {
    std::lock_guard lk(mu_);
    auto it = weights_.find(model);
    return it == weights_.end() ? 1.0 : it->second;
  }
  /** Weighted average probability; confidence = min weighted conf. */
  ModelVote fuse(std::vector<ModelVote> votes) const {
    ModelVote out; out.model = "ensemble";
    if (votes.empty()) return out;
    double num=0, den=0, confNum=0;
    for (auto& v : votes) {
      double w = weight(v.model) * std::max(0.0, v.weight);
      num += w * v.probability; den += w; confNum += w * v.confidence;
    }
    if (den <= 0) return out;
    out.probability = num/den; out.confidence = confNum/den; out.weight = den;
    return out;
  }
  double disagreement(const std::vector<ModelVote>& votes) const {
    if (votes.size() < 2) return 0;
    double mean = 0; for (auto& v: votes) mean += v.probability; mean /= votes.size();
    double var = 0; for (auto& v: votes) { double d=v.probability-mean; var+=d*d; }
    return std::sqrt(var / votes.size());
  }
private:
  mutable std::mutex mu_;
  std::unordered_map<std::string,double> weights_;
};
} // namespace crashcore
