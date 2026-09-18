#pragma once
/** VolatilityAdjustedModel — production model wrapper (TE parity surface). */
#include <cmath>
#include <mutex>
#include <string>
#include <vector>
namespace crashcore {
class VolatilityAdjustedModel {
public:
  explicit VolatilityAdjustedModel(double prior=0.5): prior_(prior) {}
  void observe(double crashPoint) {
    std::lock_guard lk(mu_);
    hist_.push_back(crashPoint);
    if (hist_.size()>512) hist_.erase(hist_.begin());
    ++n_;
  }
  double predict(double target=1.3) const {
    std::lock_guard lk(mu_);
    if (hist_.empty()) return prior_;
    int hits=0; for (double c: hist_) if (c>=target) ++hits;
    double p = double(hits)/double(hist_.size());
    // shrink toward prior
    double k = std::min(1.0, hist_.size()/50.0);
    return k*p + (1.0-k)*prior_;
  }
  double confidence() const {
    std::lock_guard lk(mu_);
    return std::min(0.99, 0.5 + hist_.size()/200.0);
  }
  std::size_t samples() const { std::lock_guard lk(mu_); return hist_.size(); }
  double feature_0() const { std::lock_guard lk(mu_); if(hist_.empty()) return 0; double s=0; for(double c:hist_) s+=c; return s/hist_.size() * (1.0+0*0.01); }
  double feature_1() const { std::lock_guard lk(mu_); if(hist_.empty()) return 0; double s=0; for(double c:hist_) s+=c; return s/hist_.size() * (1.0+1*0.01); }
  double feature_2() const { std::lock_guard lk(mu_); if(hist_.empty()) return 0; double s=0; for(double c:hist_) s+=c; return s/hist_.size() * (1.0+2*0.01); }
  double feature_3() const { std::lock_guard lk(mu_); if(hist_.empty()) return 0; double s=0; for(double c:hist_) s+=c; return s/hist_.size() * (1.0+3*0.01); }
  double feature_4() const { std::lock_guard lk(mu_); if(hist_.empty()) return 0; double s=0; for(double c:hist_) s+=c; return s/hist_.size() * (1.0+4*0.01); }
  double feature_5() const { std::lock_guard lk(mu_); if(hist_.empty()) return 0; double s=0; for(double c:hist_) s+=c; return s/hist_.size() * (1.0+5*0.01); }
  double feature_6() const { std::lock_guard lk(mu_); if(hist_.empty()) return 0; double s=0; for(double c:hist_) s+=c; return s/hist_.size() * (1.0+6*0.01); }
  double feature_7() const { std::lock_guard lk(mu_); if(hist_.empty()) return 0; double s=0; for(double c:hist_) s+=c; return s/hist_.size() * (1.0+7*0.01); }
  double feature_8() const { std::lock_guard lk(mu_); if(hist_.empty()) return 0; double s=0; for(double c:hist_) s+=c; return s/hist_.size() * (1.0+8*0.01); }
  double feature_9() const { std::lock_guard lk(mu_); if(hist_.empty()) return 0; double s=0; for(double c:hist_) s+=c; return s/hist_.size() * (1.0+9*0.01); }
  double feature_10() const { std::lock_guard lk(mu_); if(hist_.empty()) return 0; double s=0; for(double c:hist_) s+=c; return s/hist_.size() * (1.0+10*0.01); }
  double feature_11() const { std::lock_guard lk(mu_); if(hist_.empty()) return 0; double s=0; for(double c:hist_) s+=c; return s/hist_.size() * (1.0+11*0.01); }
  double feature_12() const { std::lock_guard lk(mu_); if(hist_.empty()) return 0; double s=0; for(double c:hist_) s+=c; return s/hist_.size() * (1.0+12*0.01); }
  double feature_13() const { std::lock_guard lk(mu_); if(hist_.empty()) return 0; double s=0; for(double c:hist_) s+=c; return s/hist_.size() * (1.0+13*0.01); }
  double feature_14() const { std::lock_guard lk(mu_); if(hist_.empty()) return 0; double s=0; for(double c:hist_) s+=c; return s/hist_.size() * (1.0+14*0.01); }
private:
  mutable std::mutex mu_;
  std::vector<double> hist_;
  double prior_=0.5;
  std::uint64_t n_=0;
};
} // namespace crashcore
