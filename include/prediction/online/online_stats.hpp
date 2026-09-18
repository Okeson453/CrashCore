#pragma once
#include <cmath>
#include <algorithm>
#include <vector>
#include <deque>
#include <mutex>
#include <string>
#include <unordered_map>
namespace crashcore {

class OnlineStat0 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    n_++; mean_ += (x-mean_)/n_;
    m2_ += (x-mean_)*(x-(mean_-(x-mean_)/(n_?n_:1)));
    window_.push_back(x);
    if (window_.size()>256) window_.pop_front();
  }
  double mean() const { std::lock_guard lk(mu_); return mean_; }
  double variance() const { std::lock_guard lk(mu_); return n_>1?m2_/(n_-1):0; }
  double stddev() const { return std::sqrt(variance()); }
  double ewma(double alpha=0.1) const {
    std::lock_guard lk(mu_);
    if (window_.empty()) return 0;
    double e=window_.front();
    for (size_t i=1;i<window_.size();++i) e=alpha*window_[i]+(1-alpha)*e;
    return e;
  }
  std::size_t size() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_;
  double mean_=0,m2_=0; std::size_t n_=0;
  std::deque<double> window_;
};

class OnlineStat1 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    n_++; mean_ += (x-mean_)/n_;
    m2_ += (x-mean_)*(x-(mean_-(x-mean_)/(n_?n_:1)));
    window_.push_back(x);
    if (window_.size()>256) window_.pop_front();
  }
  double mean() const { std::lock_guard lk(mu_); return mean_; }
  double variance() const { std::lock_guard lk(mu_); return n_>1?m2_/(n_-1):0; }
  double stddev() const { return std::sqrt(variance()); }
  double ewma(double alpha=0.1) const {
    std::lock_guard lk(mu_);
    if (window_.empty()) return 0;
    double e=window_.front();
    for (size_t i=1;i<window_.size();++i) e=alpha*window_[i]+(1-alpha)*e;
    return e;
  }
  std::size_t size() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_;
  double mean_=0,m2_=0; std::size_t n_=0;
  std::deque<double> window_;
};

class OnlineStat2 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    n_++; mean_ += (x-mean_)/n_;
    m2_ += (x-mean_)*(x-(mean_-(x-mean_)/(n_?n_:1)));
    window_.push_back(x);
    if (window_.size()>256) window_.pop_front();
  }
  double mean() const { std::lock_guard lk(mu_); return mean_; }
  double variance() const { std::lock_guard lk(mu_); return n_>1?m2_/(n_-1):0; }
  double stddev() const { return std::sqrt(variance()); }
  double ewma(double alpha=0.1) const {
    std::lock_guard lk(mu_);
    if (window_.empty()) return 0;
    double e=window_.front();
    for (size_t i=1;i<window_.size();++i) e=alpha*window_[i]+(1-alpha)*e;
    return e;
  }
  std::size_t size() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_;
  double mean_=0,m2_=0; std::size_t n_=0;
  std::deque<double> window_;
};

class OnlineStat3 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    n_++; mean_ += (x-mean_)/n_;
    m2_ += (x-mean_)*(x-(mean_-(x-mean_)/(n_?n_:1)));
    window_.push_back(x);
    if (window_.size()>256) window_.pop_front();
  }
  double mean() const { std::lock_guard lk(mu_); return mean_; }
  double variance() const { std::lock_guard lk(mu_); return n_>1?m2_/(n_-1):0; }
  double stddev() const { return std::sqrt(variance()); }
  double ewma(double alpha=0.1) const {
    std::lock_guard lk(mu_);
    if (window_.empty()) return 0;
    double e=window_.front();
    for (size_t i=1;i<window_.size();++i) e=alpha*window_[i]+(1-alpha)*e;
    return e;
  }
  std::size_t size() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_;
  double mean_=0,m2_=0; std::size_t n_=0;
  std::deque<double> window_;
};

class OnlineStat4 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    n_++; mean_ += (x-mean_)/n_;
    m2_ += (x-mean_)*(x-(mean_-(x-mean_)/(n_?n_:1)));
    window_.push_back(x);
    if (window_.size()>256) window_.pop_front();
  }
  double mean() const { std::lock_guard lk(mu_); return mean_; }
  double variance() const { std::lock_guard lk(mu_); return n_>1?m2_/(n_-1):0; }
  double stddev() const { return std::sqrt(variance()); }
  double ewma(double alpha=0.1) const {
    std::lock_guard lk(mu_);
    if (window_.empty()) return 0;
    double e=window_.front();
    for (size_t i=1;i<window_.size();++i) e=alpha*window_[i]+(1-alpha)*e;
    return e;
  }
  std::size_t size() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_;
  double mean_=0,m2_=0; std::size_t n_=0;
  std::deque<double> window_;
};

class OnlineStat5 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    n_++; mean_ += (x-mean_)/n_;
    m2_ += (x-mean_)*(x-(mean_-(x-mean_)/(n_?n_:1)));
    window_.push_back(x);
    if (window_.size()>256) window_.pop_front();
  }
  double mean() const { std::lock_guard lk(mu_); return mean_; }
  double variance() const { std::lock_guard lk(mu_); return n_>1?m2_/(n_-1):0; }
  double stddev() const { return std::sqrt(variance()); }
  double ewma(double alpha=0.1) const {
    std::lock_guard lk(mu_);
    if (window_.empty()) return 0;
    double e=window_.front();
    for (size_t i=1;i<window_.size();++i) e=alpha*window_[i]+(1-alpha)*e;
    return e;
  }
  std::size_t size() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_;
  double mean_=0,m2_=0; std::size_t n_=0;
  std::deque<double> window_;
};

class OnlineStat6 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    n_++; mean_ += (x-mean_)/n_;
    m2_ += (x-mean_)*(x-(mean_-(x-mean_)/(n_?n_:1)));
    window_.push_back(x);
    if (window_.size()>256) window_.pop_front();
  }
  double mean() const { std::lock_guard lk(mu_); return mean_; }
  double variance() const { std::lock_guard lk(mu_); return n_>1?m2_/(n_-1):0; }
  double stddev() const { return std::sqrt(variance()); }
  double ewma(double alpha=0.1) const {
    std::lock_guard lk(mu_);
    if (window_.empty()) return 0;
    double e=window_.front();
    for (size_t i=1;i<window_.size();++i) e=alpha*window_[i]+(1-alpha)*e;
    return e;
  }
  std::size_t size() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_;
  double mean_=0,m2_=0; std::size_t n_=0;
  std::deque<double> window_;
};

class OnlineStat7 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    n_++; mean_ += (x-mean_)/n_;
    m2_ += (x-mean_)*(x-(mean_-(x-mean_)/(n_?n_:1)));
    window_.push_back(x);
    if (window_.size()>256) window_.pop_front();
  }
  double mean() const { std::lock_guard lk(mu_); return mean_; }
  double variance() const { std::lock_guard lk(mu_); return n_>1?m2_/(n_-1):0; }
  double stddev() const { return std::sqrt(variance()); }
  double ewma(double alpha=0.1) const {
    std::lock_guard lk(mu_);
    if (window_.empty()) return 0;
    double e=window_.front();
    for (size_t i=1;i<window_.size();++i) e=alpha*window_[i]+(1-alpha)*e;
    return e;
  }
  std::size_t size() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_;
  double mean_=0,m2_=0; std::size_t n_=0;
  std::deque<double> window_;
};

class OnlineStat8 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    n_++; mean_ += (x-mean_)/n_;
    m2_ += (x-mean_)*(x-(mean_-(x-mean_)/(n_?n_:1)));
    window_.push_back(x);
    if (window_.size()>256) window_.pop_front();
  }
  double mean() const { std::lock_guard lk(mu_); return mean_; }
  double variance() const { std::lock_guard lk(mu_); return n_>1?m2_/(n_-1):0; }
  double stddev() const { return std::sqrt(variance()); }
  double ewma(double alpha=0.1) const {
    std::lock_guard lk(mu_);
    if (window_.empty()) return 0;
    double e=window_.front();
    for (size_t i=1;i<window_.size();++i) e=alpha*window_[i]+(1-alpha)*e;
    return e;
  }
  std::size_t size() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_;
  double mean_=0,m2_=0; std::size_t n_=0;
  std::deque<double> window_;
};

class OnlineStat9 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    n_++; mean_ += (x-mean_)/n_;
    m2_ += (x-mean_)*(x-(mean_-(x-mean_)/(n_?n_:1)));
    window_.push_back(x);
    if (window_.size()>256) window_.pop_front();
  }
  double mean() const { std::lock_guard lk(mu_); return mean_; }
  double variance() const { std::lock_guard lk(mu_); return n_>1?m2_/(n_-1):0; }
  double stddev() const { return std::sqrt(variance()); }
  double ewma(double alpha=0.1) const {
    std::lock_guard lk(mu_);
    if (window_.empty()) return 0;
    double e=window_.front();
    for (size_t i=1;i<window_.size();++i) e=alpha*window_[i]+(1-alpha)*e;
    return e;
  }
  std::size_t size() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_;
  double mean_=0,m2_=0; std::size_t n_=0;
  std::deque<double> window_;
};

class OnlineStat10 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    n_++; mean_ += (x-mean_)/n_;
    m2_ += (x-mean_)*(x-(mean_-(x-mean_)/(n_?n_:1)));
    window_.push_back(x);
    if (window_.size()>256) window_.pop_front();
  }
  double mean() const { std::lock_guard lk(mu_); return mean_; }
  double variance() const { std::lock_guard lk(mu_); return n_>1?m2_/(n_-1):0; }
  double stddev() const { return std::sqrt(variance()); }
  double ewma(double alpha=0.1) const {
    std::lock_guard lk(mu_);
    if (window_.empty()) return 0;
    double e=window_.front();
    for (size_t i=1;i<window_.size();++i) e=alpha*window_[i]+(1-alpha)*e;
    return e;
  }
  std::size_t size() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_;
  double mean_=0,m2_=0; std::size_t n_=0;
  std::deque<double> window_;
};

class OnlineStat11 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    n_++; mean_ += (x-mean_)/n_;
    m2_ += (x-mean_)*(x-(mean_-(x-mean_)/(n_?n_:1)));
    window_.push_back(x);
    if (window_.size()>256) window_.pop_front();
  }
  double mean() const { std::lock_guard lk(mu_); return mean_; }
  double variance() const { std::lock_guard lk(mu_); return n_>1?m2_/(n_-1):0; }
  double stddev() const { return std::sqrt(variance()); }
  double ewma(double alpha=0.1) const {
    std::lock_guard lk(mu_);
    if (window_.empty()) return 0;
    double e=window_.front();
    for (size_t i=1;i<window_.size();++i) e=alpha*window_[i]+(1-alpha)*e;
    return e;
  }
  std::size_t size() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_;
  double mean_=0,m2_=0; std::size_t n_=0;
  std::deque<double> window_;
};

class OnlineStat12 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    n_++; mean_ += (x-mean_)/n_;
    m2_ += (x-mean_)*(x-(mean_-(x-mean_)/(n_?n_:1)));
    window_.push_back(x);
    if (window_.size()>256) window_.pop_front();
  }
  double mean() const { std::lock_guard lk(mu_); return mean_; }
  double variance() const { std::lock_guard lk(mu_); return n_>1?m2_/(n_-1):0; }
  double stddev() const { return std::sqrt(variance()); }
  double ewma(double alpha=0.1) const {
    std::lock_guard lk(mu_);
    if (window_.empty()) return 0;
    double e=window_.front();
    for (size_t i=1;i<window_.size();++i) e=alpha*window_[i]+(1-alpha)*e;
    return e;
  }
  std::size_t size() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_;
  double mean_=0,m2_=0; std::size_t n_=0;
  std::deque<double> window_;
};

class OnlineStat13 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    n_++; mean_ += (x-mean_)/n_;
    m2_ += (x-mean_)*(x-(mean_-(x-mean_)/(n_?n_:1)));
    window_.push_back(x);
    if (window_.size()>256) window_.pop_front();
  }
  double mean() const { std::lock_guard lk(mu_); return mean_; }
  double variance() const { std::lock_guard lk(mu_); return n_>1?m2_/(n_-1):0; }
  double stddev() const { return std::sqrt(variance()); }
  double ewma(double alpha=0.1) const {
    std::lock_guard lk(mu_);
    if (window_.empty()) return 0;
    double e=window_.front();
    for (size_t i=1;i<window_.size();++i) e=alpha*window_[i]+(1-alpha)*e;
    return e;
  }
  std::size_t size() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_;
  double mean_=0,m2_=0; std::size_t n_=0;
  std::deque<double> window_;
};

class OnlineStat14 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    n_++; mean_ += (x-mean_)/n_;
    m2_ += (x-mean_)*(x-(mean_-(x-mean_)/(n_?n_:1)));
    window_.push_back(x);
    if (window_.size()>256) window_.pop_front();
  }
  double mean() const { std::lock_guard lk(mu_); return mean_; }
  double variance() const { std::lock_guard lk(mu_); return n_>1?m2_/(n_-1):0; }
  double stddev() const { return std::sqrt(variance()); }
  double ewma(double alpha=0.1) const {
    std::lock_guard lk(mu_);
    if (window_.empty()) return 0;
    double e=window_.front();
    for (size_t i=1;i<window_.size();++i) e=alpha*window_[i]+(1-alpha)*e;
    return e;
  }
  std::size_t size() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_;
  double mean_=0,m2_=0; std::size_t n_=0;
  std::deque<double> window_;
};

class OnlineStat15 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    n_++; mean_ += (x-mean_)/n_;
    m2_ += (x-mean_)*(x-(mean_-(x-mean_)/(n_?n_:1)));
    window_.push_back(x);
    if (window_.size()>256) window_.pop_front();
  }
  double mean() const { std::lock_guard lk(mu_); return mean_; }
  double variance() const { std::lock_guard lk(mu_); return n_>1?m2_/(n_-1):0; }
  double stddev() const { return std::sqrt(variance()); }
  double ewma(double alpha=0.1) const {
    std::lock_guard lk(mu_);
    if (window_.empty()) return 0;
    double e=window_.front();
    for (size_t i=1;i<window_.size();++i) e=alpha*window_[i]+(1-alpha)*e;
    return e;
  }
  std::size_t size() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_;
  double mean_=0,m2_=0; std::size_t n_=0;
  std::deque<double> window_;
};

class OnlineStat16 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    n_++; mean_ += (x-mean_)/n_;
    m2_ += (x-mean_)*(x-(mean_-(x-mean_)/(n_?n_:1)));
    window_.push_back(x);
    if (window_.size()>256) window_.pop_front();
  }
  double mean() const { std::lock_guard lk(mu_); return mean_; }
  double variance() const { std::lock_guard lk(mu_); return n_>1?m2_/(n_-1):0; }
  double stddev() const { return std::sqrt(variance()); }
  double ewma(double alpha=0.1) const {
    std::lock_guard lk(mu_);
    if (window_.empty()) return 0;
    double e=window_.front();
    for (size_t i=1;i<window_.size();++i) e=alpha*window_[i]+(1-alpha)*e;
    return e;
  }
  std::size_t size() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_;
  double mean_=0,m2_=0; std::size_t n_=0;
  std::deque<double> window_;
};

class OnlineStat17 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    n_++; mean_ += (x-mean_)/n_;
    m2_ += (x-mean_)*(x-(mean_-(x-mean_)/(n_?n_:1)));
    window_.push_back(x);
    if (window_.size()>256) window_.pop_front();
  }
  double mean() const { std::lock_guard lk(mu_); return mean_; }
  double variance() const { std::lock_guard lk(mu_); return n_>1?m2_/(n_-1):0; }
  double stddev() const { return std::sqrt(variance()); }
  double ewma(double alpha=0.1) const {
    std::lock_guard lk(mu_);
    if (window_.empty()) return 0;
    double e=window_.front();
    for (size_t i=1;i<window_.size();++i) e=alpha*window_[i]+(1-alpha)*e;
    return e;
  }
  std::size_t size() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_;
  double mean_=0,m2_=0; std::size_t n_=0;
  std::deque<double> window_;
};

class OnlineStat18 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    n_++; mean_ += (x-mean_)/n_;
    m2_ += (x-mean_)*(x-(mean_-(x-mean_)/(n_?n_:1)));
    window_.push_back(x);
    if (window_.size()>256) window_.pop_front();
  }
  double mean() const { std::lock_guard lk(mu_); return mean_; }
  double variance() const { std::lock_guard lk(mu_); return n_>1?m2_/(n_-1):0; }
  double stddev() const { return std::sqrt(variance()); }
  double ewma(double alpha=0.1) const {
    std::lock_guard lk(mu_);
    if (window_.empty()) return 0;
    double e=window_.front();
    for (size_t i=1;i<window_.size();++i) e=alpha*window_[i]+(1-alpha)*e;
    return e;
  }
  std::size_t size() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_;
  double mean_=0,m2_=0; std::size_t n_=0;
  std::deque<double> window_;
};

class OnlineStat19 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    n_++; mean_ += (x-mean_)/n_;
    m2_ += (x-mean_)*(x-(mean_-(x-mean_)/(n_?n_:1)));
    window_.push_back(x);
    if (window_.size()>256) window_.pop_front();
  }
  double mean() const { std::lock_guard lk(mu_); return mean_; }
  double variance() const { std::lock_guard lk(mu_); return n_>1?m2_/(n_-1):0; }
  double stddev() const { return std::sqrt(variance()); }
  double ewma(double alpha=0.1) const {
    std::lock_guard lk(mu_);
    if (window_.empty()) return 0;
    double e=window_.front();
    for (size_t i=1;i<window_.size();++i) e=alpha*window_[i]+(1-alpha)*e;
    return e;
  }
  std::size_t size() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_;
  double mean_=0,m2_=0; std::size_t n_=0;
  std::deque<double> window_;
};

class OnlineStat20 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    n_++; mean_ += (x-mean_)/n_;
    m2_ += (x-mean_)*(x-(mean_-(x-mean_)/(n_?n_:1)));
    window_.push_back(x);
    if (window_.size()>256) window_.pop_front();
  }
  double mean() const { std::lock_guard lk(mu_); return mean_; }
  double variance() const { std::lock_guard lk(mu_); return n_>1?m2_/(n_-1):0; }
  double stddev() const { return std::sqrt(variance()); }
  double ewma(double alpha=0.1) const {
    std::lock_guard lk(mu_);
    if (window_.empty()) return 0;
    double e=window_.front();
    for (size_t i=1;i<window_.size();++i) e=alpha*window_[i]+(1-alpha)*e;
    return e;
  }
  std::size_t size() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_;
  double mean_=0,m2_=0; std::size_t n_=0;
  std::deque<double> window_;
};

class OnlineStat21 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    n_++; mean_ += (x-mean_)/n_;
    m2_ += (x-mean_)*(x-(mean_-(x-mean_)/(n_?n_:1)));
    window_.push_back(x);
    if (window_.size()>256) window_.pop_front();
  }
  double mean() const { std::lock_guard lk(mu_); return mean_; }
  double variance() const { std::lock_guard lk(mu_); return n_>1?m2_/(n_-1):0; }
  double stddev() const { return std::sqrt(variance()); }
  double ewma(double alpha=0.1) const {
    std::lock_guard lk(mu_);
    if (window_.empty()) return 0;
    double e=window_.front();
    for (size_t i=1;i<window_.size();++i) e=alpha*window_[i]+(1-alpha)*e;
    return e;
  }
  std::size_t size() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_;
  double mean_=0,m2_=0; std::size_t n_=0;
  std::deque<double> window_;
};

class OnlineStat22 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    n_++; mean_ += (x-mean_)/n_;
    m2_ += (x-mean_)*(x-(mean_-(x-mean_)/(n_?n_:1)));
    window_.push_back(x);
    if (window_.size()>256) window_.pop_front();
  }
  double mean() const { std::lock_guard lk(mu_); return mean_; }
  double variance() const { std::lock_guard lk(mu_); return n_>1?m2_/(n_-1):0; }
  double stddev() const { return std::sqrt(variance()); }
  double ewma(double alpha=0.1) const {
    std::lock_guard lk(mu_);
    if (window_.empty()) return 0;
    double e=window_.front();
    for (size_t i=1;i<window_.size();++i) e=alpha*window_[i]+(1-alpha)*e;
    return e;
  }
  std::size_t size() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_;
  double mean_=0,m2_=0; std::size_t n_=0;
  std::deque<double> window_;
};

class OnlineStat23 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    n_++; mean_ += (x-mean_)/n_;
    m2_ += (x-mean_)*(x-(mean_-(x-mean_)/(n_?n_:1)));
    window_.push_back(x);
    if (window_.size()>256) window_.pop_front();
  }
  double mean() const { std::lock_guard lk(mu_); return mean_; }
  double variance() const { std::lock_guard lk(mu_); return n_>1?m2_/(n_-1):0; }
  double stddev() const { return std::sqrt(variance()); }
  double ewma(double alpha=0.1) const {
    std::lock_guard lk(mu_);
    if (window_.empty()) return 0;
    double e=window_.front();
    for (size_t i=1;i<window_.size();++i) e=alpha*window_[i]+(1-alpha)*e;
    return e;
  }
  std::size_t size() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_;
  double mean_=0,m2_=0; std::size_t n_=0;
  std::deque<double> window_;
};

class OnlineStat24 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    n_++; mean_ += (x-mean_)/n_;
    m2_ += (x-mean_)*(x-(mean_-(x-mean_)/(n_?n_:1)));
    window_.push_back(x);
    if (window_.size()>256) window_.pop_front();
  }
  double mean() const { std::lock_guard lk(mu_); return mean_; }
  double variance() const { std::lock_guard lk(mu_); return n_>1?m2_/(n_-1):0; }
  double stddev() const { return std::sqrt(variance()); }
  double ewma(double alpha=0.1) const {
    std::lock_guard lk(mu_);
    if (window_.empty()) return 0;
    double e=window_.front();
    for (size_t i=1;i<window_.size();++i) e=alpha*window_[i]+(1-alpha)*e;
    return e;
  }
  std::size_t size() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_;
  double mean_=0,m2_=0; std::size_t n_=0;
  std::deque<double> window_;
};

class OnlineStat25 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    n_++; mean_ += (x-mean_)/n_;
    m2_ += (x-mean_)*(x-(mean_-(x-mean_)/(n_?n_:1)));
    window_.push_back(x);
    if (window_.size()>256) window_.pop_front();
  }
  double mean() const { std::lock_guard lk(mu_); return mean_; }
  double variance() const { std::lock_guard lk(mu_); return n_>1?m2_/(n_-1):0; }
  double stddev() const { return std::sqrt(variance()); }
  double ewma(double alpha=0.1) const {
    std::lock_guard lk(mu_);
    if (window_.empty()) return 0;
    double e=window_.front();
    for (size_t i=1;i<window_.size();++i) e=alpha*window_[i]+(1-alpha)*e;
    return e;
  }
  std::size_t size() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_;
  double mean_=0,m2_=0; std::size_t n_=0;
  std::deque<double> window_;
};

class OnlineStat26 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    n_++; mean_ += (x-mean_)/n_;
    m2_ += (x-mean_)*(x-(mean_-(x-mean_)/(n_?n_:1)));
    window_.push_back(x);
    if (window_.size()>256) window_.pop_front();
  }
  double mean() const { std::lock_guard lk(mu_); return mean_; }
  double variance() const { std::lock_guard lk(mu_); return n_>1?m2_/(n_-1):0; }
  double stddev() const { return std::sqrt(variance()); }
  double ewma(double alpha=0.1) const {
    std::lock_guard lk(mu_);
    if (window_.empty()) return 0;
    double e=window_.front();
    for (size_t i=1;i<window_.size();++i) e=alpha*window_[i]+(1-alpha)*e;
    return e;
  }
  std::size_t size() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_;
  double mean_=0,m2_=0; std::size_t n_=0;
  std::deque<double> window_;
};

class OnlineStat27 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    n_++; mean_ += (x-mean_)/n_;
    m2_ += (x-mean_)*(x-(mean_-(x-mean_)/(n_?n_:1)));
    window_.push_back(x);
    if (window_.size()>256) window_.pop_front();
  }
  double mean() const { std::lock_guard lk(mu_); return mean_; }
  double variance() const { std::lock_guard lk(mu_); return n_>1?m2_/(n_-1):0; }
  double stddev() const { return std::sqrt(variance()); }
  double ewma(double alpha=0.1) const {
    std::lock_guard lk(mu_);
    if (window_.empty()) return 0;
    double e=window_.front();
    for (size_t i=1;i<window_.size();++i) e=alpha*window_[i]+(1-alpha)*e;
    return e;
  }
  std::size_t size() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_;
  double mean_=0,m2_=0; std::size_t n_=0;
  std::deque<double> window_;
};

class OnlineStat28 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    n_++; mean_ += (x-mean_)/n_;
    m2_ += (x-mean_)*(x-(mean_-(x-mean_)/(n_?n_:1)));
    window_.push_back(x);
    if (window_.size()>256) window_.pop_front();
  }
  double mean() const { std::lock_guard lk(mu_); return mean_; }
  double variance() const { std::lock_guard lk(mu_); return n_>1?m2_/(n_-1):0; }
  double stddev() const { return std::sqrt(variance()); }
  double ewma(double alpha=0.1) const {
    std::lock_guard lk(mu_);
    if (window_.empty()) return 0;
    double e=window_.front();
    for (size_t i=1;i<window_.size();++i) e=alpha*window_[i]+(1-alpha)*e;
    return e;
  }
  std::size_t size() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_;
  double mean_=0,m2_=0; std::size_t n_=0;
  std::deque<double> window_;
};

class OnlineStat29 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    n_++; mean_ += (x-mean_)/n_;
    m2_ += (x-mean_)*(x-(mean_-(x-mean_)/(n_?n_:1)));
    window_.push_back(x);
    if (window_.size()>256) window_.pop_front();
  }
  double mean() const { std::lock_guard lk(mu_); return mean_; }
  double variance() const { std::lock_guard lk(mu_); return n_>1?m2_/(n_-1):0; }
  double stddev() const { return std::sqrt(variance()); }
  double ewma(double alpha=0.1) const {
    std::lock_guard lk(mu_);
    if (window_.empty()) return 0;
    double e=window_.front();
    for (size_t i=1;i<window_.size();++i) e=alpha*window_[i]+(1-alpha)*e;
    return e;
  }
  std::size_t size() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_;
  double mean_=0,m2_=0; std::size_t n_=0;
  std::deque<double> window_;
};

class OnlineStat30 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    n_++; mean_ += (x-mean_)/n_;
    m2_ += (x-mean_)*(x-(mean_-(x-mean_)/(n_?n_:1)));
    window_.push_back(x);
    if (window_.size()>256) window_.pop_front();
  }
  double mean() const { std::lock_guard lk(mu_); return mean_; }
  double variance() const { std::lock_guard lk(mu_); return n_>1?m2_/(n_-1):0; }
  double stddev() const { return std::sqrt(variance()); }
  double ewma(double alpha=0.1) const {
    std::lock_guard lk(mu_);
    if (window_.empty()) return 0;
    double e=window_.front();
    for (size_t i=1;i<window_.size();++i) e=alpha*window_[i]+(1-alpha)*e;
    return e;
  }
  std::size_t size() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_;
  double mean_=0,m2_=0; std::size_t n_=0;
  std::deque<double> window_;
};

class OnlineStat31 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    n_++; mean_ += (x-mean_)/n_;
    m2_ += (x-mean_)*(x-(mean_-(x-mean_)/(n_?n_:1)));
    window_.push_back(x);
    if (window_.size()>256) window_.pop_front();
  }
  double mean() const { std::lock_guard lk(mu_); return mean_; }
  double variance() const { std::lock_guard lk(mu_); return n_>1?m2_/(n_-1):0; }
  double stddev() const { return std::sqrt(variance()); }
  double ewma(double alpha=0.1) const {
    std::lock_guard lk(mu_);
    if (window_.empty()) return 0;
    double e=window_.front();
    for (size_t i=1;i<window_.size();++i) e=alpha*window_[i]+(1-alpha)*e;
    return e;
  }
  std::size_t size() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_;
  double mean_=0,m2_=0; std::size_t n_=0;
  std::deque<double> window_;
};

class OnlineStat32 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    n_++; mean_ += (x-mean_)/n_;
    m2_ += (x-mean_)*(x-(mean_-(x-mean_)/(n_?n_:1)));
    window_.push_back(x);
    if (window_.size()>256) window_.pop_front();
  }
  double mean() const { std::lock_guard lk(mu_); return mean_; }
  double variance() const { std::lock_guard lk(mu_); return n_>1?m2_/(n_-1):0; }
  double stddev() const { return std::sqrt(variance()); }
  double ewma(double alpha=0.1) const {
    std::lock_guard lk(mu_);
    if (window_.empty()) return 0;
    double e=window_.front();
    for (size_t i=1;i<window_.size();++i) e=alpha*window_[i]+(1-alpha)*e;
    return e;
  }
  std::size_t size() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_;
  double mean_=0,m2_=0; std::size_t n_=0;
  std::deque<double> window_;
};

class OnlineStat33 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    n_++; mean_ += (x-mean_)/n_;
    m2_ += (x-mean_)*(x-(mean_-(x-mean_)/(n_?n_:1)));
    window_.push_back(x);
    if (window_.size()>256) window_.pop_front();
  }
  double mean() const { std::lock_guard lk(mu_); return mean_; }
  double variance() const { std::lock_guard lk(mu_); return n_>1?m2_/(n_-1):0; }
  double stddev() const { return std::sqrt(variance()); }
  double ewma(double alpha=0.1) const {
    std::lock_guard lk(mu_);
    if (window_.empty()) return 0;
    double e=window_.front();
    for (size_t i=1;i<window_.size();++i) e=alpha*window_[i]+(1-alpha)*e;
    return e;
  }
  std::size_t size() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_;
  double mean_=0,m2_=0; std::size_t n_=0;
  std::deque<double> window_;
};

class OnlineStat34 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    n_++; mean_ += (x-mean_)/n_;
    m2_ += (x-mean_)*(x-(mean_-(x-mean_)/(n_?n_:1)));
    window_.push_back(x);
    if (window_.size()>256) window_.pop_front();
  }
  double mean() const { std::lock_guard lk(mu_); return mean_; }
  double variance() const { std::lock_guard lk(mu_); return n_>1?m2_/(n_-1):0; }
  double stddev() const { return std::sqrt(variance()); }
  double ewma(double alpha=0.1) const {
    std::lock_guard lk(mu_);
    if (window_.empty()) return 0;
    double e=window_.front();
    for (size_t i=1;i<window_.size();++i) e=alpha*window_[i]+(1-alpha)*e;
    return e;
  }
  std::size_t size() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_;
  double mean_=0,m2_=0; std::size_t n_=0;
  std::deque<double> window_;
};

class OnlineStat35 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    n_++; mean_ += (x-mean_)/n_;
    m2_ += (x-mean_)*(x-(mean_-(x-mean_)/(n_?n_:1)));
    window_.push_back(x);
    if (window_.size()>256) window_.pop_front();
  }
  double mean() const { std::lock_guard lk(mu_); return mean_; }
  double variance() const { std::lock_guard lk(mu_); return n_>1?m2_/(n_-1):0; }
  double stddev() const { return std::sqrt(variance()); }
  double ewma(double alpha=0.1) const {
    std::lock_guard lk(mu_);
    if (window_.empty()) return 0;
    double e=window_.front();
    for (size_t i=1;i<window_.size();++i) e=alpha*window_[i]+(1-alpha)*e;
    return e;
  }
  std::size_t size() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_;
  double mean_=0,m2_=0; std::size_t n_=0;
  std::deque<double> window_;
};

class OnlineStat36 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    n_++; mean_ += (x-mean_)/n_;
    m2_ += (x-mean_)*(x-(mean_-(x-mean_)/(n_?n_:1)));
    window_.push_back(x);
    if (window_.size()>256) window_.pop_front();
  }
  double mean() const { std::lock_guard lk(mu_); return mean_; }
  double variance() const { std::lock_guard lk(mu_); return n_>1?m2_/(n_-1):0; }
  double stddev() const { return std::sqrt(variance()); }
  double ewma(double alpha=0.1) const {
    std::lock_guard lk(mu_);
    if (window_.empty()) return 0;
    double e=window_.front();
    for (size_t i=1;i<window_.size();++i) e=alpha*window_[i]+(1-alpha)*e;
    return e;
  }
  std::size_t size() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_;
  double mean_=0,m2_=0; std::size_t n_=0;
  std::deque<double> window_;
};

class OnlineStat37 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    n_++; mean_ += (x-mean_)/n_;
    m2_ += (x-mean_)*(x-(mean_-(x-mean_)/(n_?n_:1)));
    window_.push_back(x);
    if (window_.size()>256) window_.pop_front();
  }
  double mean() const { std::lock_guard lk(mu_); return mean_; }
  double variance() const { std::lock_guard lk(mu_); return n_>1?m2_/(n_-1):0; }
  double stddev() const { return std::sqrt(variance()); }
  double ewma(double alpha=0.1) const {
    std::lock_guard lk(mu_);
    if (window_.empty()) return 0;
    double e=window_.front();
    for (size_t i=1;i<window_.size();++i) e=alpha*window_[i]+(1-alpha)*e;
    return e;
  }
  std::size_t size() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_;
  double mean_=0,m2_=0; std::size_t n_=0;
  std::deque<double> window_;
};

class OnlineStat38 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    n_++; mean_ += (x-mean_)/n_;
    m2_ += (x-mean_)*(x-(mean_-(x-mean_)/(n_?n_:1)));
    window_.push_back(x);
    if (window_.size()>256) window_.pop_front();
  }
  double mean() const { std::lock_guard lk(mu_); return mean_; }
  double variance() const { std::lock_guard lk(mu_); return n_>1?m2_/(n_-1):0; }
  double stddev() const { return std::sqrt(variance()); }
  double ewma(double alpha=0.1) const {
    std::lock_guard lk(mu_);
    if (window_.empty()) return 0;
    double e=window_.front();
    for (size_t i=1;i<window_.size();++i) e=alpha*window_[i]+(1-alpha)*e;
    return e;
  }
  std::size_t size() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_;
  double mean_=0,m2_=0; std::size_t n_=0;
  std::deque<double> window_;
};

class OnlineStat39 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    n_++; mean_ += (x-mean_)/n_;
    m2_ += (x-mean_)*(x-(mean_-(x-mean_)/(n_?n_:1)));
    window_.push_back(x);
    if (window_.size()>256) window_.pop_front();
  }
  double mean() const { std::lock_guard lk(mu_); return mean_; }
  double variance() const { std::lock_guard lk(mu_); return n_>1?m2_/(n_-1):0; }
  double stddev() const { return std::sqrt(variance()); }
  double ewma(double alpha=0.1) const {
    std::lock_guard lk(mu_);
    if (window_.empty()) return 0;
    double e=window_.front();
    for (size_t i=1;i<window_.size();++i) e=alpha*window_[i]+(1-alpha)*e;
    return e;
  }
  std::size_t size() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_;
  double mean_=0,m2_=0; std::size_t n_=0;
  std::deque<double> window_;
};

class OnlineStat40 {
public:
  void add(double x) { std::lock_guard lk(mu_); n_++; mean_+=(x-mean_)/n_; window_.push_back(x); if(window_.size()>128) window_.pop_front(); }
  double mean() const { std::lock_guard lk(mu_); return mean_; }
  double min() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x<m)m=x; return m; }
  double max() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x>m)m=x; return m; }
  double percentile(double p) const {
    std::lock_guard lk(mu_);
    if(window_.empty()) return 0;
    std::vector<double> v(window_.begin(), window_.end());
    std::sort(v.begin(), v.end());
    size_t i = size_t(p * (v.size()-1));
    return v[i];
  }
private:
  mutable std::mutex mu_; double mean_=0; std::size_t n_=0; std::deque<double> window_;
};

class OnlineStat41 {
public:
  void add(double x) { std::lock_guard lk(mu_); n_++; mean_+=(x-mean_)/n_; window_.push_back(x); if(window_.size()>128) window_.pop_front(); }
  double mean() const { std::lock_guard lk(mu_); return mean_; }
  double min() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x<m)m=x; return m; }
  double max() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x>m)m=x; return m; }
  double percentile(double p) const {
    std::lock_guard lk(mu_);
    if(window_.empty()) return 0;
    std::vector<double> v(window_.begin(), window_.end());
    std::sort(v.begin(), v.end());
    size_t i = size_t(p * (v.size()-1));
    return v[i];
  }
private:
  mutable std::mutex mu_; double mean_=0; std::size_t n_=0; std::deque<double> window_;
};

class OnlineStat42 {
public:
  void add(double x) { std::lock_guard lk(mu_); n_++; mean_+=(x-mean_)/n_; window_.push_back(x); if(window_.size()>128) window_.pop_front(); }
  double mean() const { std::lock_guard lk(mu_); return mean_; }
  double min() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x<m)m=x; return m; }
  double max() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x>m)m=x; return m; }
  double percentile(double p) const {
    std::lock_guard lk(mu_);
    if(window_.empty()) return 0;
    std::vector<double> v(window_.begin(), window_.end());
    std::sort(v.begin(), v.end());
    size_t i = size_t(p * (v.size()-1));
    return v[i];
  }
private:
  mutable std::mutex mu_; double mean_=0; std::size_t n_=0; std::deque<double> window_;
};

class OnlineStat43 {
public:
  void add(double x) { std::lock_guard lk(mu_); n_++; mean_+=(x-mean_)/n_; window_.push_back(x); if(window_.size()>128) window_.pop_front(); }
  double mean() const { std::lock_guard lk(mu_); return mean_; }
  double min() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x<m)m=x; return m; }
  double max() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x>m)m=x; return m; }
  double percentile(double p) const {
    std::lock_guard lk(mu_);
    if(window_.empty()) return 0;
    std::vector<double> v(window_.begin(), window_.end());
    std::sort(v.begin(), v.end());
    size_t i = size_t(p * (v.size()-1));
    return v[i];
  }
private:
  mutable std::mutex mu_; double mean_=0; std::size_t n_=0; std::deque<double> window_;
};

class OnlineStat44 {
public:
  void add(double x) { std::lock_guard lk(mu_); n_++; mean_+=(x-mean_)/n_; window_.push_back(x); if(window_.size()>128) window_.pop_front(); }
  double mean() const { std::lock_guard lk(mu_); return mean_; }
  double min() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x<m)m=x; return m; }
  double max() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x>m)m=x; return m; }
  double percentile(double p) const {
    std::lock_guard lk(mu_);
    if(window_.empty()) return 0;
    std::vector<double> v(window_.begin(), window_.end());
    std::sort(v.begin(), v.end());
    size_t i = size_t(p * (v.size()-1));
    return v[i];
  }
private:
  mutable std::mutex mu_; double mean_=0; std::size_t n_=0; std::deque<double> window_;
};

class OnlineStat45 {
public:
  void add(double x) { std::lock_guard lk(mu_); n_++; mean_+=(x-mean_)/n_; window_.push_back(x); if(window_.size()>128) window_.pop_front(); }
  double mean() const { std::lock_guard lk(mu_); return mean_; }
  double min() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x<m)m=x; return m; }
  double max() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x>m)m=x; return m; }
  double percentile(double p) const {
    std::lock_guard lk(mu_);
    if(window_.empty()) return 0;
    std::vector<double> v(window_.begin(), window_.end());
    std::sort(v.begin(), v.end());
    size_t i = size_t(p * (v.size()-1));
    return v[i];
  }
private:
  mutable std::mutex mu_; double mean_=0; std::size_t n_=0; std::deque<double> window_;
};

class OnlineStat46 {
public:
  void add(double x) { std::lock_guard lk(mu_); n_++; mean_+=(x-mean_)/n_; window_.push_back(x); if(window_.size()>128) window_.pop_front(); }
  double mean() const { std::lock_guard lk(mu_); return mean_; }
  double min() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x<m)m=x; return m; }
  double max() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x>m)m=x; return m; }
  double percentile(double p) const {
    std::lock_guard lk(mu_);
    if(window_.empty()) return 0;
    std::vector<double> v(window_.begin(), window_.end());
    std::sort(v.begin(), v.end());
    size_t i = size_t(p * (v.size()-1));
    return v[i];
  }
private:
  mutable std::mutex mu_; double mean_=0; std::size_t n_=0; std::deque<double> window_;
};

class OnlineStat47 {
public:
  void add(double x) { std::lock_guard lk(mu_); n_++; mean_+=(x-mean_)/n_; window_.push_back(x); if(window_.size()>128) window_.pop_front(); }
  double mean() const { std::lock_guard lk(mu_); return mean_; }
  double min() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x<m)m=x; return m; }
  double max() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x>m)m=x; return m; }
  double percentile(double p) const {
    std::lock_guard lk(mu_);
    if(window_.empty()) return 0;
    std::vector<double> v(window_.begin(), window_.end());
    std::sort(v.begin(), v.end());
    size_t i = size_t(p * (v.size()-1));
    return v[i];
  }
private:
  mutable std::mutex mu_; double mean_=0; std::size_t n_=0; std::deque<double> window_;
};

class OnlineStat48 {
public:
  void add(double x) { std::lock_guard lk(mu_); n_++; mean_+=(x-mean_)/n_; window_.push_back(x); if(window_.size()>128) window_.pop_front(); }
  double mean() const { std::lock_guard lk(mu_); return mean_; }
  double min() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x<m)m=x; return m; }
  double max() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x>m)m=x; return m; }
  double percentile(double p) const {
    std::lock_guard lk(mu_);
    if(window_.empty()) return 0;
    std::vector<double> v(window_.begin(), window_.end());
    std::sort(v.begin(), v.end());
    size_t i = size_t(p * (v.size()-1));
    return v[i];
  }
private:
  mutable std::mutex mu_; double mean_=0; std::size_t n_=0; std::deque<double> window_;
};

class OnlineStat49 {
public:
  void add(double x) { std::lock_guard lk(mu_); n_++; mean_+=(x-mean_)/n_; window_.push_back(x); if(window_.size()>128) window_.pop_front(); }
  double mean() const { std::lock_guard lk(mu_); return mean_; }
  double min() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x<m)m=x; return m; }
  double max() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x>m)m=x; return m; }
  double percentile(double p) const {
    std::lock_guard lk(mu_);
    if(window_.empty()) return 0;
    std::vector<double> v(window_.begin(), window_.end());
    std::sort(v.begin(), v.end());
    size_t i = size_t(p * (v.size()-1));
    return v[i];
  }
private:
  mutable std::mutex mu_; double mean_=0; std::size_t n_=0; std::deque<double> window_;
};

class OnlineStat50 {
public:
  void add(double x) { std::lock_guard lk(mu_); n_++; mean_+=(x-mean_)/n_; window_.push_back(x); if(window_.size()>128) window_.pop_front(); }
  double mean() const { std::lock_guard lk(mu_); return mean_; }
  double min() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x<m)m=x; return m; }
  double max() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x>m)m=x; return m; }
  double percentile(double p) const {
    std::lock_guard lk(mu_);
    if(window_.empty()) return 0;
    std::vector<double> v(window_.begin(), window_.end());
    std::sort(v.begin(), v.end());
    size_t i = size_t(p * (v.size()-1));
    return v[i];
  }
private:
  mutable std::mutex mu_; double mean_=0; std::size_t n_=0; std::deque<double> window_;
};

class OnlineStat51 {
public:
  void add(double x) { std::lock_guard lk(mu_); n_++; mean_+=(x-mean_)/n_; window_.push_back(x); if(window_.size()>128) window_.pop_front(); }
  double mean() const { std::lock_guard lk(mu_); return mean_; }
  double min() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x<m)m=x; return m; }
  double max() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x>m)m=x; return m; }
  double percentile(double p) const {
    std::lock_guard lk(mu_);
    if(window_.empty()) return 0;
    std::vector<double> v(window_.begin(), window_.end());
    std::sort(v.begin(), v.end());
    size_t i = size_t(p * (v.size()-1));
    return v[i];
  }
private:
  mutable std::mutex mu_; double mean_=0; std::size_t n_=0; std::deque<double> window_;
};

class OnlineStat52 {
public:
  void add(double x) { std::lock_guard lk(mu_); n_++; mean_+=(x-mean_)/n_; window_.push_back(x); if(window_.size()>128) window_.pop_front(); }
  double mean() const { std::lock_guard lk(mu_); return mean_; }
  double min() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x<m)m=x; return m; }
  double max() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x>m)m=x; return m; }
  double percentile(double p) const {
    std::lock_guard lk(mu_);
    if(window_.empty()) return 0;
    std::vector<double> v(window_.begin(), window_.end());
    std::sort(v.begin(), v.end());
    size_t i = size_t(p * (v.size()-1));
    return v[i];
  }
private:
  mutable std::mutex mu_; double mean_=0; std::size_t n_=0; std::deque<double> window_;
};

class OnlineStat53 {
public:
  void add(double x) { std::lock_guard lk(mu_); n_++; mean_+=(x-mean_)/n_; window_.push_back(x); if(window_.size()>128) window_.pop_front(); }
  double mean() const { std::lock_guard lk(mu_); return mean_; }
  double min() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x<m)m=x; return m; }
  double max() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x>m)m=x; return m; }
  double percentile(double p) const {
    std::lock_guard lk(mu_);
    if(window_.empty()) return 0;
    std::vector<double> v(window_.begin(), window_.end());
    std::sort(v.begin(), v.end());
    size_t i = size_t(p * (v.size()-1));
    return v[i];
  }
private:
  mutable std::mutex mu_; double mean_=0; std::size_t n_=0; std::deque<double> window_;
};

class OnlineStat54 {
public:
  void add(double x) { std::lock_guard lk(mu_); n_++; mean_+=(x-mean_)/n_; window_.push_back(x); if(window_.size()>128) window_.pop_front(); }
  double mean() const { std::lock_guard lk(mu_); return mean_; }
  double min() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x<m)m=x; return m; }
  double max() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x>m)m=x; return m; }
  double percentile(double p) const {
    std::lock_guard lk(mu_);
    if(window_.empty()) return 0;
    std::vector<double> v(window_.begin(), window_.end());
    std::sort(v.begin(), v.end());
    size_t i = size_t(p * (v.size()-1));
    return v[i];
  }
private:
  mutable std::mutex mu_; double mean_=0; std::size_t n_=0; std::deque<double> window_;
};

class OnlineStat55 {
public:
  void add(double x) { std::lock_guard lk(mu_); n_++; mean_+=(x-mean_)/n_; window_.push_back(x); if(window_.size()>128) window_.pop_front(); }
  double mean() const { std::lock_guard lk(mu_); return mean_; }
  double min() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x<m)m=x; return m; }
  double max() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x>m)m=x; return m; }
  double percentile(double p) const {
    std::lock_guard lk(mu_);
    if(window_.empty()) return 0;
    std::vector<double> v(window_.begin(), window_.end());
    std::sort(v.begin(), v.end());
    size_t i = size_t(p * (v.size()-1));
    return v[i];
  }
private:
  mutable std::mutex mu_; double mean_=0; std::size_t n_=0; std::deque<double> window_;
};

class OnlineStat56 {
public:
  void add(double x) { std::lock_guard lk(mu_); n_++; mean_+=(x-mean_)/n_; window_.push_back(x); if(window_.size()>128) window_.pop_front(); }
  double mean() const { std::lock_guard lk(mu_); return mean_; }
  double min() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x<m)m=x; return m; }
  double max() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x>m)m=x; return m; }
  double percentile(double p) const {
    std::lock_guard lk(mu_);
    if(window_.empty()) return 0;
    std::vector<double> v(window_.begin(), window_.end());
    std::sort(v.begin(), v.end());
    size_t i = size_t(p * (v.size()-1));
    return v[i];
  }
private:
  mutable std::mutex mu_; double mean_=0; std::size_t n_=0; std::deque<double> window_;
};

class OnlineStat57 {
public:
  void add(double x) { std::lock_guard lk(mu_); n_++; mean_+=(x-mean_)/n_; window_.push_back(x); if(window_.size()>128) window_.pop_front(); }
  double mean() const { std::lock_guard lk(mu_); return mean_; }
  double min() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x<m)m=x; return m; }
  double max() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x>m)m=x; return m; }
  double percentile(double p) const {
    std::lock_guard lk(mu_);
    if(window_.empty()) return 0;
    std::vector<double> v(window_.begin(), window_.end());
    std::sort(v.begin(), v.end());
    size_t i = size_t(p * (v.size()-1));
    return v[i];
  }
private:
  mutable std::mutex mu_; double mean_=0; std::size_t n_=0; std::deque<double> window_;
};

class OnlineStat58 {
public:
  void add(double x) { std::lock_guard lk(mu_); n_++; mean_+=(x-mean_)/n_; window_.push_back(x); if(window_.size()>128) window_.pop_front(); }
  double mean() const { std::lock_guard lk(mu_); return mean_; }
  double min() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x<m)m=x; return m; }
  double max() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x>m)m=x; return m; }
  double percentile(double p) const {
    std::lock_guard lk(mu_);
    if(window_.empty()) return 0;
    std::vector<double> v(window_.begin(), window_.end());
    std::sort(v.begin(), v.end());
    size_t i = size_t(p * (v.size()-1));
    return v[i];
  }
private:
  mutable std::mutex mu_; double mean_=0; std::size_t n_=0; std::deque<double> window_;
};

class OnlineStat59 {
public:
  void add(double x) { std::lock_guard lk(mu_); n_++; mean_+=(x-mean_)/n_; window_.push_back(x); if(window_.size()>128) window_.pop_front(); }
  double mean() const { std::lock_guard lk(mu_); return mean_; }
  double min() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x<m)m=x; return m; }
  double max() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x>m)m=x; return m; }
  double percentile(double p) const {
    std::lock_guard lk(mu_);
    if(window_.empty()) return 0;
    std::vector<double> v(window_.begin(), window_.end());
    std::sort(v.begin(), v.end());
    size_t i = size_t(p * (v.size()-1));
    return v[i];
  }
private:
  mutable std::mutex mu_; double mean_=0; std::size_t n_=0; std::deque<double> window_;
};

class OnlineStat60 {
public:
  void add(double x) { std::lock_guard lk(mu_); n_++; mean_+=(x-mean_)/n_; window_.push_back(x); if(window_.size()>128) window_.pop_front(); }
  double mean() const { std::lock_guard lk(mu_); return mean_; }
  double min() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x<m)m=x; return m; }
  double max() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x>m)m=x; return m; }
  double percentile(double p) const {
    std::lock_guard lk(mu_);
    if(window_.empty()) return 0;
    std::vector<double> v(window_.begin(), window_.end());
    std::sort(v.begin(), v.end());
    size_t i = size_t(p * (v.size()-1));
    return v[i];
  }
private:
  mutable std::mutex mu_; double mean_=0; std::size_t n_=0; std::deque<double> window_;
};

class OnlineStat61 {
public:
  void add(double x) { std::lock_guard lk(mu_); n_++; mean_+=(x-mean_)/n_; window_.push_back(x); if(window_.size()>128) window_.pop_front(); }
  double mean() const { std::lock_guard lk(mu_); return mean_; }
  double min() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x<m)m=x; return m; }
  double max() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x>m)m=x; return m; }
  double percentile(double p) const {
    std::lock_guard lk(mu_);
    if(window_.empty()) return 0;
    std::vector<double> v(window_.begin(), window_.end());
    std::sort(v.begin(), v.end());
    size_t i = size_t(p * (v.size()-1));
    return v[i];
  }
private:
  mutable std::mutex mu_; double mean_=0; std::size_t n_=0; std::deque<double> window_;
};

class OnlineStat62 {
public:
  void add(double x) { std::lock_guard lk(mu_); n_++; mean_+=(x-mean_)/n_; window_.push_back(x); if(window_.size()>128) window_.pop_front(); }
  double mean() const { std::lock_guard lk(mu_); return mean_; }
  double min() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x<m)m=x; return m; }
  double max() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x>m)m=x; return m; }
  double percentile(double p) const {
    std::lock_guard lk(mu_);
    if(window_.empty()) return 0;
    std::vector<double> v(window_.begin(), window_.end());
    std::sort(v.begin(), v.end());
    size_t i = size_t(p * (v.size()-1));
    return v[i];
  }
private:
  mutable std::mutex mu_; double mean_=0; std::size_t n_=0; std::deque<double> window_;
};

class OnlineStat63 {
public:
  void add(double x) { std::lock_guard lk(mu_); n_++; mean_+=(x-mean_)/n_; window_.push_back(x); if(window_.size()>128) window_.pop_front(); }
  double mean() const { std::lock_guard lk(mu_); return mean_; }
  double min() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x<m)m=x; return m; }
  double max() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x>m)m=x; return m; }
  double percentile(double p) const {
    std::lock_guard lk(mu_);
    if(window_.empty()) return 0;
    std::vector<double> v(window_.begin(), window_.end());
    std::sort(v.begin(), v.end());
    size_t i = size_t(p * (v.size()-1));
    return v[i];
  }
private:
  mutable std::mutex mu_; double mean_=0; std::size_t n_=0; std::deque<double> window_;
};

class OnlineStat64 {
public:
  void add(double x) { std::lock_guard lk(mu_); n_++; mean_+=(x-mean_)/n_; window_.push_back(x); if(window_.size()>128) window_.pop_front(); }
  double mean() const { std::lock_guard lk(mu_); return mean_; }
  double min() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x<m)m=x; return m; }
  double max() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x>m)m=x; return m; }
  double percentile(double p) const {
    std::lock_guard lk(mu_);
    if(window_.empty()) return 0;
    std::vector<double> v(window_.begin(), window_.end());
    std::sort(v.begin(), v.end());
    size_t i = size_t(p * (v.size()-1));
    return v[i];
  }
private:
  mutable std::mutex mu_; double mean_=0; std::size_t n_=0; std::deque<double> window_;
};

class OnlineStat65 {
public:
  void add(double x) { std::lock_guard lk(mu_); n_++; mean_+=(x-mean_)/n_; window_.push_back(x); if(window_.size()>128) window_.pop_front(); }
  double mean() const { std::lock_guard lk(mu_); return mean_; }
  double min() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x<m)m=x; return m; }
  double max() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x>m)m=x; return m; }
  double percentile(double p) const {
    std::lock_guard lk(mu_);
    if(window_.empty()) return 0;
    std::vector<double> v(window_.begin(), window_.end());
    std::sort(v.begin(), v.end());
    size_t i = size_t(p * (v.size()-1));
    return v[i];
  }
private:
  mutable std::mutex mu_; double mean_=0; std::size_t n_=0; std::deque<double> window_;
};

class OnlineStat66 {
public:
  void add(double x) { std::lock_guard lk(mu_); n_++; mean_+=(x-mean_)/n_; window_.push_back(x); if(window_.size()>128) window_.pop_front(); }
  double mean() const { std::lock_guard lk(mu_); return mean_; }
  double min() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x<m)m=x; return m; }
  double max() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x>m)m=x; return m; }
  double percentile(double p) const {
    std::lock_guard lk(mu_);
    if(window_.empty()) return 0;
    std::vector<double> v(window_.begin(), window_.end());
    std::sort(v.begin(), v.end());
    size_t i = size_t(p * (v.size()-1));
    return v[i];
  }
private:
  mutable std::mutex mu_; double mean_=0; std::size_t n_=0; std::deque<double> window_;
};

class OnlineStat67 {
public:
  void add(double x) { std::lock_guard lk(mu_); n_++; mean_+=(x-mean_)/n_; window_.push_back(x); if(window_.size()>128) window_.pop_front(); }
  double mean() const { std::lock_guard lk(mu_); return mean_; }
  double min() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x<m)m=x; return m; }
  double max() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x>m)m=x; return m; }
  double percentile(double p) const {
    std::lock_guard lk(mu_);
    if(window_.empty()) return 0;
    std::vector<double> v(window_.begin(), window_.end());
    std::sort(v.begin(), v.end());
    size_t i = size_t(p * (v.size()-1));
    return v[i];
  }
private:
  mutable std::mutex mu_; double mean_=0; std::size_t n_=0; std::deque<double> window_;
};

class OnlineStat68 {
public:
  void add(double x) { std::lock_guard lk(mu_); n_++; mean_+=(x-mean_)/n_; window_.push_back(x); if(window_.size()>128) window_.pop_front(); }
  double mean() const { std::lock_guard lk(mu_); return mean_; }
  double min() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x<m)m=x; return m; }
  double max() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x>m)m=x; return m; }
  double percentile(double p) const {
    std::lock_guard lk(mu_);
    if(window_.empty()) return 0;
    std::vector<double> v(window_.begin(), window_.end());
    std::sort(v.begin(), v.end());
    size_t i = size_t(p * (v.size()-1));
    return v[i];
  }
private:
  mutable std::mutex mu_; double mean_=0; std::size_t n_=0; std::deque<double> window_;
};

class OnlineStat69 {
public:
  void add(double x) { std::lock_guard lk(mu_); n_++; mean_+=(x-mean_)/n_; window_.push_back(x); if(window_.size()>128) window_.pop_front(); }
  double mean() const { std::lock_guard lk(mu_); return mean_; }
  double min() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x<m)m=x; return m; }
  double max() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x>m)m=x; return m; }
  double percentile(double p) const {
    std::lock_guard lk(mu_);
    if(window_.empty()) return 0;
    std::vector<double> v(window_.begin(), window_.end());
    std::sort(v.begin(), v.end());
    size_t i = size_t(p * (v.size()-1));
    return v[i];
  }
private:
  mutable std::mutex mu_; double mean_=0; std::size_t n_=0; std::deque<double> window_;
};

class OnlineStat70 {
public:
  void add(double x) { std::lock_guard lk(mu_); n_++; mean_+=(x-mean_)/n_; window_.push_back(x); if(window_.size()>128) window_.pop_front(); }
  double mean() const { std::lock_guard lk(mu_); return mean_; }
  double min() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x<m)m=x; return m; }
  double max() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x>m)m=x; return m; }
  double percentile(double p) const {
    std::lock_guard lk(mu_);
    if(window_.empty()) return 0;
    std::vector<double> v(window_.begin(), window_.end());
    std::sort(v.begin(), v.end());
    size_t i = size_t(p * (v.size()-1));
    return v[i];
  }
private:
  mutable std::mutex mu_; double mean_=0; std::size_t n_=0; std::deque<double> window_;
};

class OnlineStat71 {
public:
  void add(double x) { std::lock_guard lk(mu_); n_++; mean_+=(x-mean_)/n_; window_.push_back(x); if(window_.size()>128) window_.pop_front(); }
  double mean() const { std::lock_guard lk(mu_); return mean_; }
  double min() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x<m)m=x; return m; }
  double max() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x>m)m=x; return m; }
  double percentile(double p) const {
    std::lock_guard lk(mu_);
    if(window_.empty()) return 0;
    std::vector<double> v(window_.begin(), window_.end());
    std::sort(v.begin(), v.end());
    size_t i = size_t(p * (v.size()-1));
    return v[i];
  }
private:
  mutable std::mutex mu_; double mean_=0; std::size_t n_=0; std::deque<double> window_;
};

class OnlineStat72 {
public:
  void add(double x) { std::lock_guard lk(mu_); n_++; mean_+=(x-mean_)/n_; window_.push_back(x); if(window_.size()>128) window_.pop_front(); }
  double mean() const { std::lock_guard lk(mu_); return mean_; }
  double min() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x<m)m=x; return m; }
  double max() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x>m)m=x; return m; }
  double percentile(double p) const {
    std::lock_guard lk(mu_);
    if(window_.empty()) return 0;
    std::vector<double> v(window_.begin(), window_.end());
    std::sort(v.begin(), v.end());
    size_t i = size_t(p * (v.size()-1));
    return v[i];
  }
private:
  mutable std::mutex mu_; double mean_=0; std::size_t n_=0; std::deque<double> window_;
};

class OnlineStat73 {
public:
  void add(double x) { std::lock_guard lk(mu_); n_++; mean_+=(x-mean_)/n_; window_.push_back(x); if(window_.size()>128) window_.pop_front(); }
  double mean() const { std::lock_guard lk(mu_); return mean_; }
  double min() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x<m)m=x; return m; }
  double max() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x>m)m=x; return m; }
  double percentile(double p) const {
    std::lock_guard lk(mu_);
    if(window_.empty()) return 0;
    std::vector<double> v(window_.begin(), window_.end());
    std::sort(v.begin(), v.end());
    size_t i = size_t(p * (v.size()-1));
    return v[i];
  }
private:
  mutable std::mutex mu_; double mean_=0; std::size_t n_=0; std::deque<double> window_;
};

class OnlineStat74 {
public:
  void add(double x) { std::lock_guard lk(mu_); n_++; mean_+=(x-mean_)/n_; window_.push_back(x); if(window_.size()>128) window_.pop_front(); }
  double mean() const { std::lock_guard lk(mu_); return mean_; }
  double min() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x<m)m=x; return m; }
  double max() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x>m)m=x; return m; }
  double percentile(double p) const {
    std::lock_guard lk(mu_);
    if(window_.empty()) return 0;
    std::vector<double> v(window_.begin(), window_.end());
    std::sort(v.begin(), v.end());
    size_t i = size_t(p * (v.size()-1));
    return v[i];
  }
private:
  mutable std::mutex mu_; double mean_=0; std::size_t n_=0; std::deque<double> window_;
};

class OnlineStat75 {
public:
  void add(double x) { std::lock_guard lk(mu_); n_++; mean_+=(x-mean_)/n_; window_.push_back(x); if(window_.size()>128) window_.pop_front(); }
  double mean() const { std::lock_guard lk(mu_); return mean_; }
  double min() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x<m)m=x; return m; }
  double max() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x>m)m=x; return m; }
  double percentile(double p) const {
    std::lock_guard lk(mu_);
    if(window_.empty()) return 0;
    std::vector<double> v(window_.begin(), window_.end());
    std::sort(v.begin(), v.end());
    size_t i = size_t(p * (v.size()-1));
    return v[i];
  }
private:
  mutable std::mutex mu_; double mean_=0; std::size_t n_=0; std::deque<double> window_;
};

class OnlineStat76 {
public:
  void add(double x) { std::lock_guard lk(mu_); n_++; mean_+=(x-mean_)/n_; window_.push_back(x); if(window_.size()>128) window_.pop_front(); }
  double mean() const { std::lock_guard lk(mu_); return mean_; }
  double min() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x<m)m=x; return m; }
  double max() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x>m)m=x; return m; }
  double percentile(double p) const {
    std::lock_guard lk(mu_);
    if(window_.empty()) return 0;
    std::vector<double> v(window_.begin(), window_.end());
    std::sort(v.begin(), v.end());
    size_t i = size_t(p * (v.size()-1));
    return v[i];
  }
private:
  mutable std::mutex mu_; double mean_=0; std::size_t n_=0; std::deque<double> window_;
};

class OnlineStat77 {
public:
  void add(double x) { std::lock_guard lk(mu_); n_++; mean_+=(x-mean_)/n_; window_.push_back(x); if(window_.size()>128) window_.pop_front(); }
  double mean() const { std::lock_guard lk(mu_); return mean_; }
  double min() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x<m)m=x; return m; }
  double max() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x>m)m=x; return m; }
  double percentile(double p) const {
    std::lock_guard lk(mu_);
    if(window_.empty()) return 0;
    std::vector<double> v(window_.begin(), window_.end());
    std::sort(v.begin(), v.end());
    size_t i = size_t(p * (v.size()-1));
    return v[i];
  }
private:
  mutable std::mutex mu_; double mean_=0; std::size_t n_=0; std::deque<double> window_;
};

class OnlineStat78 {
public:
  void add(double x) { std::lock_guard lk(mu_); n_++; mean_+=(x-mean_)/n_; window_.push_back(x); if(window_.size()>128) window_.pop_front(); }
  double mean() const { std::lock_guard lk(mu_); return mean_; }
  double min() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x<m)m=x; return m; }
  double max() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x>m)m=x; return m; }
  double percentile(double p) const {
    std::lock_guard lk(mu_);
    if(window_.empty()) return 0;
    std::vector<double> v(window_.begin(), window_.end());
    std::sort(v.begin(), v.end());
    size_t i = size_t(p * (v.size()-1));
    return v[i];
  }
private:
  mutable std::mutex mu_; double mean_=0; std::size_t n_=0; std::deque<double> window_;
};

class OnlineStat79 {
public:
  void add(double x) { std::lock_guard lk(mu_); n_++; mean_+=(x-mean_)/n_; window_.push_back(x); if(window_.size()>128) window_.pop_front(); }
  double mean() const { std::lock_guard lk(mu_); return mean_; }
  double min() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x<m)m=x; return m; }
  double max() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x>m)m=x; return m; }
  double percentile(double p) const {
    std::lock_guard lk(mu_);
    if(window_.empty()) return 0;
    std::vector<double> v(window_.begin(), window_.end());
    std::sort(v.begin(), v.end());
    size_t i = size_t(p * (v.size()-1));
    return v[i];
  }
private:
  mutable std::mutex mu_; double mean_=0; std::size_t n_=0; std::deque<double> window_;
};

class OnlineStat80 {
public:
  void add(double x) { std::lock_guard lk(mu_); n_++; mean_+=(x-mean_)/n_; window_.push_back(x); if(window_.size()>128) window_.pop_front(); }
  double mean() const { std::lock_guard lk(mu_); return mean_; }
  double min() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x<m)m=x; return m; }
  double max() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x>m)m=x; return m; }
  double percentile(double p) const {
    std::lock_guard lk(mu_);
    if(window_.empty()) return 0;
    std::vector<double> v(window_.begin(), window_.end());
    std::sort(v.begin(), v.end());
    size_t i = size_t(p * (v.size()-1));
    return v[i];
  }
private:
  mutable std::mutex mu_; double mean_=0; std::size_t n_=0; std::deque<double> window_;
};

class OnlineStat81 {
public:
  void add(double x) { std::lock_guard lk(mu_); n_++; mean_+=(x-mean_)/n_; window_.push_back(x); if(window_.size()>128) window_.pop_front(); }
  double mean() const { std::lock_guard lk(mu_); return mean_; }
  double min() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x<m)m=x; return m; }
  double max() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x>m)m=x; return m; }
  double percentile(double p) const {
    std::lock_guard lk(mu_);
    if(window_.empty()) return 0;
    std::vector<double> v(window_.begin(), window_.end());
    std::sort(v.begin(), v.end());
    size_t i = size_t(p * (v.size()-1));
    return v[i];
  }
private:
  mutable std::mutex mu_; double mean_=0; std::size_t n_=0; std::deque<double> window_;
};

class OnlineStat82 {
public:
  void add(double x) { std::lock_guard lk(mu_); n_++; mean_+=(x-mean_)/n_; window_.push_back(x); if(window_.size()>128) window_.pop_front(); }
  double mean() const { std::lock_guard lk(mu_); return mean_; }
  double min() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x<m)m=x; return m; }
  double max() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x>m)m=x; return m; }
  double percentile(double p) const {
    std::lock_guard lk(mu_);
    if(window_.empty()) return 0;
    std::vector<double> v(window_.begin(), window_.end());
    std::sort(v.begin(), v.end());
    size_t i = size_t(p * (v.size()-1));
    return v[i];
  }
private:
  mutable std::mutex mu_; double mean_=0; std::size_t n_=0; std::deque<double> window_;
};

class OnlineStat83 {
public:
  void add(double x) { std::lock_guard lk(mu_); n_++; mean_+=(x-mean_)/n_; window_.push_back(x); if(window_.size()>128) window_.pop_front(); }
  double mean() const { std::lock_guard lk(mu_); return mean_; }
  double min() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x<m)m=x; return m; }
  double max() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x>m)m=x; return m; }
  double percentile(double p) const {
    std::lock_guard lk(mu_);
    if(window_.empty()) return 0;
    std::vector<double> v(window_.begin(), window_.end());
    std::sort(v.begin(), v.end());
    size_t i = size_t(p * (v.size()-1));
    return v[i];
  }
private:
  mutable std::mutex mu_; double mean_=0; std::size_t n_=0; std::deque<double> window_;
};

class OnlineStat84 {
public:
  void add(double x) { std::lock_guard lk(mu_); n_++; mean_+=(x-mean_)/n_; window_.push_back(x); if(window_.size()>128) window_.pop_front(); }
  double mean() const { std::lock_guard lk(mu_); return mean_; }
  double min() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x<m)m=x; return m; }
  double max() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x>m)m=x; return m; }
  double percentile(double p) const {
    std::lock_guard lk(mu_);
    if(window_.empty()) return 0;
    std::vector<double> v(window_.begin(), window_.end());
    std::sort(v.begin(), v.end());
    size_t i = size_t(p * (v.size()-1));
    return v[i];
  }
private:
  mutable std::mutex mu_; double mean_=0; std::size_t n_=0; std::deque<double> window_;
};

class OnlineStat85 {
public:
  void add(double x) { std::lock_guard lk(mu_); n_++; mean_+=(x-mean_)/n_; window_.push_back(x); if(window_.size()>128) window_.pop_front(); }
  double mean() const { std::lock_guard lk(mu_); return mean_; }
  double min() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x<m)m=x; return m; }
  double max() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x>m)m=x; return m; }
  double percentile(double p) const {
    std::lock_guard lk(mu_);
    if(window_.empty()) return 0;
    std::vector<double> v(window_.begin(), window_.end());
    std::sort(v.begin(), v.end());
    size_t i = size_t(p * (v.size()-1));
    return v[i];
  }
private:
  mutable std::mutex mu_; double mean_=0; std::size_t n_=0; std::deque<double> window_;
};

class OnlineStat86 {
public:
  void add(double x) { std::lock_guard lk(mu_); n_++; mean_+=(x-mean_)/n_; window_.push_back(x); if(window_.size()>128) window_.pop_front(); }
  double mean() const { std::lock_guard lk(mu_); return mean_; }
  double min() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x<m)m=x; return m; }
  double max() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x>m)m=x; return m; }
  double percentile(double p) const {
    std::lock_guard lk(mu_);
    if(window_.empty()) return 0;
    std::vector<double> v(window_.begin(), window_.end());
    std::sort(v.begin(), v.end());
    size_t i = size_t(p * (v.size()-1));
    return v[i];
  }
private:
  mutable std::mutex mu_; double mean_=0; std::size_t n_=0; std::deque<double> window_;
};

class OnlineStat87 {
public:
  void add(double x) { std::lock_guard lk(mu_); n_++; mean_+=(x-mean_)/n_; window_.push_back(x); if(window_.size()>128) window_.pop_front(); }
  double mean() const { std::lock_guard lk(mu_); return mean_; }
  double min() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x<m)m=x; return m; }
  double max() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x>m)m=x; return m; }
  double percentile(double p) const {
    std::lock_guard lk(mu_);
    if(window_.empty()) return 0;
    std::vector<double> v(window_.begin(), window_.end());
    std::sort(v.begin(), v.end());
    size_t i = size_t(p * (v.size()-1));
    return v[i];
  }
private:
  mutable std::mutex mu_; double mean_=0; std::size_t n_=0; std::deque<double> window_;
};

class OnlineStat88 {
public:
  void add(double x) { std::lock_guard lk(mu_); n_++; mean_+=(x-mean_)/n_; window_.push_back(x); if(window_.size()>128) window_.pop_front(); }
  double mean() const { std::lock_guard lk(mu_); return mean_; }
  double min() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x<m)m=x; return m; }
  double max() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x>m)m=x; return m; }
  double percentile(double p) const {
    std::lock_guard lk(mu_);
    if(window_.empty()) return 0;
    std::vector<double> v(window_.begin(), window_.end());
    std::sort(v.begin(), v.end());
    size_t i = size_t(p * (v.size()-1));
    return v[i];
  }
private:
  mutable std::mutex mu_; double mean_=0; std::size_t n_=0; std::deque<double> window_;
};

class OnlineStat89 {
public:
  void add(double x) { std::lock_guard lk(mu_); n_++; mean_+=(x-mean_)/n_; window_.push_back(x); if(window_.size()>128) window_.pop_front(); }
  double mean() const { std::lock_guard lk(mu_); return mean_; }
  double min() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x<m)m=x; return m; }
  double max() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x>m)m=x; return m; }
  double percentile(double p) const {
    std::lock_guard lk(mu_);
    if(window_.empty()) return 0;
    std::vector<double> v(window_.begin(), window_.end());
    std::sort(v.begin(), v.end());
    size_t i = size_t(p * (v.size()-1));
    return v[i];
  }
private:
  mutable std::mutex mu_; double mean_=0; std::size_t n_=0; std::deque<double> window_;
};

class OnlineStat90 {
public:
  void add(double x) { std::lock_guard lk(mu_); n_++; mean_+=(x-mean_)/n_; window_.push_back(x); if(window_.size()>128) window_.pop_front(); }
  double mean() const { std::lock_guard lk(mu_); return mean_; }
  double min() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x<m)m=x; return m; }
  double max() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x>m)m=x; return m; }
  double percentile(double p) const {
    std::lock_guard lk(mu_);
    if(window_.empty()) return 0;
    std::vector<double> v(window_.begin(), window_.end());
    std::sort(v.begin(), v.end());
    size_t i = size_t(p * (v.size()-1));
    return v[i];
  }
private:
  mutable std::mutex mu_; double mean_=0; std::size_t n_=0; std::deque<double> window_;
};

class OnlineStat91 {
public:
  void add(double x) { std::lock_guard lk(mu_); n_++; mean_+=(x-mean_)/n_; window_.push_back(x); if(window_.size()>128) window_.pop_front(); }
  double mean() const { std::lock_guard lk(mu_); return mean_; }
  double min() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x<m)m=x; return m; }
  double max() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x>m)m=x; return m; }
  double percentile(double p) const {
    std::lock_guard lk(mu_);
    if(window_.empty()) return 0;
    std::vector<double> v(window_.begin(), window_.end());
    std::sort(v.begin(), v.end());
    size_t i = size_t(p * (v.size()-1));
    return v[i];
  }
private:
  mutable std::mutex mu_; double mean_=0; std::size_t n_=0; std::deque<double> window_;
};

class OnlineStat92 {
public:
  void add(double x) { std::lock_guard lk(mu_); n_++; mean_+=(x-mean_)/n_; window_.push_back(x); if(window_.size()>128) window_.pop_front(); }
  double mean() const { std::lock_guard lk(mu_); return mean_; }
  double min() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x<m)m=x; return m; }
  double max() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x>m)m=x; return m; }
  double percentile(double p) const {
    std::lock_guard lk(mu_);
    if(window_.empty()) return 0;
    std::vector<double> v(window_.begin(), window_.end());
    std::sort(v.begin(), v.end());
    size_t i = size_t(p * (v.size()-1));
    return v[i];
  }
private:
  mutable std::mutex mu_; double mean_=0; std::size_t n_=0; std::deque<double> window_;
};

class OnlineStat93 {
public:
  void add(double x) { std::lock_guard lk(mu_); n_++; mean_+=(x-mean_)/n_; window_.push_back(x); if(window_.size()>128) window_.pop_front(); }
  double mean() const { std::lock_guard lk(mu_); return mean_; }
  double min() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x<m)m=x; return m; }
  double max() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x>m)m=x; return m; }
  double percentile(double p) const {
    std::lock_guard lk(mu_);
    if(window_.empty()) return 0;
    std::vector<double> v(window_.begin(), window_.end());
    std::sort(v.begin(), v.end());
    size_t i = size_t(p * (v.size()-1));
    return v[i];
  }
private:
  mutable std::mutex mu_; double mean_=0; std::size_t n_=0; std::deque<double> window_;
};

class OnlineStat94 {
public:
  void add(double x) { std::lock_guard lk(mu_); n_++; mean_+=(x-mean_)/n_; window_.push_back(x); if(window_.size()>128) window_.pop_front(); }
  double mean() const { std::lock_guard lk(mu_); return mean_; }
  double min() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x<m)m=x; return m; }
  double max() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x>m)m=x; return m; }
  double percentile(double p) const {
    std::lock_guard lk(mu_);
    if(window_.empty()) return 0;
    std::vector<double> v(window_.begin(), window_.end());
    std::sort(v.begin(), v.end());
    size_t i = size_t(p * (v.size()-1));
    return v[i];
  }
private:
  mutable std::mutex mu_; double mean_=0; std::size_t n_=0; std::deque<double> window_;
};

class OnlineStat95 {
public:
  void add(double x) { std::lock_guard lk(mu_); n_++; mean_+=(x-mean_)/n_; window_.push_back(x); if(window_.size()>128) window_.pop_front(); }
  double mean() const { std::lock_guard lk(mu_); return mean_; }
  double min() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x<m)m=x; return m; }
  double max() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x>m)m=x; return m; }
  double percentile(double p) const {
    std::lock_guard lk(mu_);
    if(window_.empty()) return 0;
    std::vector<double> v(window_.begin(), window_.end());
    std::sort(v.begin(), v.end());
    size_t i = size_t(p * (v.size()-1));
    return v[i];
  }
private:
  mutable std::mutex mu_; double mean_=0; std::size_t n_=0; std::deque<double> window_;
};

class OnlineStat96 {
public:
  void add(double x) { std::lock_guard lk(mu_); n_++; mean_+=(x-mean_)/n_; window_.push_back(x); if(window_.size()>128) window_.pop_front(); }
  double mean() const { std::lock_guard lk(mu_); return mean_; }
  double min() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x<m)m=x; return m; }
  double max() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x>m)m=x; return m; }
  double percentile(double p) const {
    std::lock_guard lk(mu_);
    if(window_.empty()) return 0;
    std::vector<double> v(window_.begin(), window_.end());
    std::sort(v.begin(), v.end());
    size_t i = size_t(p * (v.size()-1));
    return v[i];
  }
private:
  mutable std::mutex mu_; double mean_=0; std::size_t n_=0; std::deque<double> window_;
};

class OnlineStat97 {
public:
  void add(double x) { std::lock_guard lk(mu_); n_++; mean_+=(x-mean_)/n_; window_.push_back(x); if(window_.size()>128) window_.pop_front(); }
  double mean() const { std::lock_guard lk(mu_); return mean_; }
  double min() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x<m)m=x; return m; }
  double max() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x>m)m=x; return m; }
  double percentile(double p) const {
    std::lock_guard lk(mu_);
    if(window_.empty()) return 0;
    std::vector<double> v(window_.begin(), window_.end());
    std::sort(v.begin(), v.end());
    size_t i = size_t(p * (v.size()-1));
    return v[i];
  }
private:
  mutable std::mutex mu_; double mean_=0; std::size_t n_=0; std::deque<double> window_;
};

class OnlineStat98 {
public:
  void add(double x) { std::lock_guard lk(mu_); n_++; mean_+=(x-mean_)/n_; window_.push_back(x); if(window_.size()>128) window_.pop_front(); }
  double mean() const { std::lock_guard lk(mu_); return mean_; }
  double min() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x<m)m=x; return m; }
  double max() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x>m)m=x; return m; }
  double percentile(double p) const {
    std::lock_guard lk(mu_);
    if(window_.empty()) return 0;
    std::vector<double> v(window_.begin(), window_.end());
    std::sort(v.begin(), v.end());
    size_t i = size_t(p * (v.size()-1));
    return v[i];
  }
private:
  mutable std::mutex mu_; double mean_=0; std::size_t n_=0; std::deque<double> window_;
};

class OnlineStat99 {
public:
  void add(double x) { std::lock_guard lk(mu_); n_++; mean_+=(x-mean_)/n_; window_.push_back(x); if(window_.size()>128) window_.pop_front(); }
  double mean() const { std::lock_guard lk(mu_); return mean_; }
  double min() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x<m)m=x; return m; }
  double max() const { std::lock_guard lk(mu_); if(window_.empty())return 0; double m=window_[0]; for(double x:window_) if(x>m)m=x; return m; }
  double percentile(double p) const {
    std::lock_guard lk(mu_);
    if(window_.empty()) return 0;
    std::vector<double> v(window_.begin(), window_.end());
    std::sort(v.begin(), v.end());
    size_t i = size_t(p * (v.size()-1));
    return v[i];
  }
private:
  mutable std::mutex mu_; double mean_=0; std::size_t n_=0; std::deque<double> window_;
};
} // namespace crashcore
