#pragma once
#include <algorithm>
#include <mutex>
#include <vector>
namespace crashcore {
class IsotonicCalibrator {
public:
  void fit(std::vector<double> scores, std::vector<int> labels) {
    std::lock_guard lk(mu_);
    if (scores.size()!=labels.size()||scores.empty()) return;
    std::vector<std::pair<double,int>> pairs;
    for (size_t i=0;i<scores.size();++i) pairs.push_back({scores[i],labels[i]});
    std::sort(pairs.begin(), pairs.end());
    // Pool Adjacent Violators (simplified)
    bins_.clear();
    for (auto& p : pairs) bins_.push_back({p.first, double(p.second)});
    for (size_t i=1;i<bins_.size();++i) {
      if (bins_[i].second < bins_[i-1].second) {
        double avg = (bins_[i].second + bins_[i-1].second)/2.0;
        bins_[i].second = bins_[i-1].second = avg;
      }
    }
    fitted_ = true;
  }
  double calibrate(double score) const {
    std::lock_guard lk(mu_);
    if (!fitted_ || bins_.empty()) return score;
    // piecewise constant
    for (size_t i=0;i<bins_.size();++i) {
      if (score <= bins_[i].first) return bins_[i].second;
    }
    return bins_.back().second;
  }
private:
  mutable std::mutex mu_;
  std::vector<std::pair<double,double>> bins_;
  bool fitted_=false;
};
} // namespace crashcore
