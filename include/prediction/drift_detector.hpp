#pragma once
/** Simple feature/prediction drift detector. */
#include <cmath>
#include <mutex>
#include <string>
#include <unordered_map>
#include <deque>
namespace crashcore {
class DriftDetector {
public:
  explicit DriftDetector(std::size_t window=500, double threshold=0.15)
    : window_(window), threshold_(threshold) {}
  void observe(const std::string& feature, double value) {
    std::lock_guard lk(mu_);
    auto& q = series_[feature];
    q.push_back(value);
    while (q.size() > window_) q.pop_front();
  }
  /** Population mean shift vs first half vs second half. */
  double score(const std::string& feature) const {
    std::lock_guard lk(mu_);
    auto it = series_.find(feature);
    if (it==series_.end() || it->second.size() < 20) return 0;
    const auto& q = it->second;
    const size_t mid = q.size()/2;
    double m1=0,m2=0;
    for (size_t i=0;i<mid;++i) m1+=q[i]; m1/=mid;
    for (size_t i=mid;i<q.size();++i) m2+=q[i]; m2/=(q.size()-mid);
    double denom = std::abs(m1)+1e-9;
    return std::abs(m2-m1)/denom;
  }
  bool drifted(const std::string& feature) const {
    return score(feature) > threshold_;
  }
private:
  std::size_t window_; double threshold_;
  mutable std::mutex mu_;
  std::unordered_map<std::string, std::deque<double>> series_;
};
} // namespace crashcore
