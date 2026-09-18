#pragma once
#include <vector>
#include <deque>
#include <cmath>
#include <algorithm>
#include <mutex>
namespace crashcore {

class Histograms0 {
public:
  explicit Histograms0(std::size_t cap=256): cap_(cap) {}
  void push(double x) { std::lock_guard lk(mu_); data_.push_back(x); while(data_.size()>cap_) data_.pop_front(); }
  double sum() const { std::lock_guard lk(mu_); double s=0; for(double x:data_)s+=x; return s; }
  double mean() const { auto n=size(); return n?sum()/n:0; }
  std::size_t size() const { std::lock_guard lk(mu_); return data_.size(); }
  double at(std::size_t i) const { std::lock_guard lk(mu_); return i<data_.size()?data_[i]:0; }
private:
  std::size_t cap_; mutable std::mutex mu_; std::deque<double> data_;
};

class Histograms1 {
public:
  explicit Histograms1(std::size_t cap=256): cap_(cap) {}
  void push(double x) { std::lock_guard lk(mu_); data_.push_back(x); while(data_.size()>cap_) data_.pop_front(); }
  double sum() const { std::lock_guard lk(mu_); double s=0; for(double x:data_)s+=x; return s; }
  double mean() const { auto n=size(); return n?sum()/n:0; }
  std::size_t size() const { std::lock_guard lk(mu_); return data_.size(); }
  double at(std::size_t i) const { std::lock_guard lk(mu_); return i<data_.size()?data_[i]:0; }
private:
  std::size_t cap_; mutable std::mutex mu_; std::deque<double> data_;
};

class Histograms2 {
public:
  explicit Histograms2(std::size_t cap=256): cap_(cap) {}
  void push(double x) { std::lock_guard lk(mu_); data_.push_back(x); while(data_.size()>cap_) data_.pop_front(); }
  double sum() const { std::lock_guard lk(mu_); double s=0; for(double x:data_)s+=x; return s; }
  double mean() const { auto n=size(); return n?sum()/n:0; }
  std::size_t size() const { std::lock_guard lk(mu_); return data_.size(); }
  double at(std::size_t i) const { std::lock_guard lk(mu_); return i<data_.size()?data_[i]:0; }
private:
  std::size_t cap_; mutable std::mutex mu_; std::deque<double> data_;
};

class Histograms3 {
public:
  explicit Histograms3(std::size_t cap=256): cap_(cap) {}
  void push(double x) { std::lock_guard lk(mu_); data_.push_back(x); while(data_.size()>cap_) data_.pop_front(); }
  double sum() const { std::lock_guard lk(mu_); double s=0; for(double x:data_)s+=x; return s; }
  double mean() const { auto n=size(); return n?sum()/n:0; }
  std::size_t size() const { std::lock_guard lk(mu_); return data_.size(); }
  double at(std::size_t i) const { std::lock_guard lk(mu_); return i<data_.size()?data_[i]:0; }
private:
  std::size_t cap_; mutable std::mutex mu_; std::deque<double> data_;
};

class Histograms4 {
public:
  explicit Histograms4(std::size_t cap=256): cap_(cap) {}
  void push(double x) { std::lock_guard lk(mu_); data_.push_back(x); while(data_.size()>cap_) data_.pop_front(); }
  double sum() const { std::lock_guard lk(mu_); double s=0; for(double x:data_)s+=x; return s; }
  double mean() const { auto n=size(); return n?sum()/n:0; }
  std::size_t size() const { std::lock_guard lk(mu_); return data_.size(); }
  double at(std::size_t i) const { std::lock_guard lk(mu_); return i<data_.size()?data_[i]:0; }
private:
  std::size_t cap_; mutable std::mutex mu_; std::deque<double> data_;
};

class Histograms5 {
public:
  explicit Histograms5(std::size_t cap=256): cap_(cap) {}
  void push(double x) { std::lock_guard lk(mu_); data_.push_back(x); while(data_.size()>cap_) data_.pop_front(); }
  double sum() const { std::lock_guard lk(mu_); double s=0; for(double x:data_)s+=x; return s; }
  double mean() const { auto n=size(); return n?sum()/n:0; }
  std::size_t size() const { std::lock_guard lk(mu_); return data_.size(); }
  double at(std::size_t i) const { std::lock_guard lk(mu_); return i<data_.size()?data_[i]:0; }
private:
  std::size_t cap_; mutable std::mutex mu_; std::deque<double> data_;
};

class Histograms6 {
public:
  explicit Histograms6(std::size_t cap=256): cap_(cap) {}
  void push(double x) { std::lock_guard lk(mu_); data_.push_back(x); while(data_.size()>cap_) data_.pop_front(); }
  double sum() const { std::lock_guard lk(mu_); double s=0; for(double x:data_)s+=x; return s; }
  double mean() const { auto n=size(); return n?sum()/n:0; }
  std::size_t size() const { std::lock_guard lk(mu_); return data_.size(); }
  double at(std::size_t i) const { std::lock_guard lk(mu_); return i<data_.size()?data_[i]:0; }
private:
  std::size_t cap_; mutable std::mutex mu_; std::deque<double> data_;
};

class Histograms7 {
public:
  explicit Histograms7(std::size_t cap=256): cap_(cap) {}
  void push(double x) { std::lock_guard lk(mu_); data_.push_back(x); while(data_.size()>cap_) data_.pop_front(); }
  double sum() const { std::lock_guard lk(mu_); double s=0; for(double x:data_)s+=x; return s; }
  double mean() const { auto n=size(); return n?sum()/n:0; }
  std::size_t size() const { std::lock_guard lk(mu_); return data_.size(); }
  double at(std::size_t i) const { std::lock_guard lk(mu_); return i<data_.size()?data_[i]:0; }
private:
  std::size_t cap_; mutable std::mutex mu_; std::deque<double> data_;
};

class Histograms8 {
public:
  explicit Histograms8(std::size_t cap=256): cap_(cap) {}
  void push(double x) { std::lock_guard lk(mu_); data_.push_back(x); while(data_.size()>cap_) data_.pop_front(); }
  double sum() const { std::lock_guard lk(mu_); double s=0; for(double x:data_)s+=x; return s; }
  double mean() const { auto n=size(); return n?sum()/n:0; }
  std::size_t size() const { std::lock_guard lk(mu_); return data_.size(); }
  double at(std::size_t i) const { std::lock_guard lk(mu_); return i<data_.size()?data_[i]:0; }
private:
  std::size_t cap_; mutable std::mutex mu_; std::deque<double> data_;
};

class Histograms9 {
public:
  explicit Histograms9(std::size_t cap=256): cap_(cap) {}
  void push(double x) { std::lock_guard lk(mu_); data_.push_back(x); while(data_.size()>cap_) data_.pop_front(); }
  double sum() const { std::lock_guard lk(mu_); double s=0; for(double x:data_)s+=x; return s; }
  double mean() const { auto n=size(); return n?sum()/n:0; }
  std::size_t size() const { std::lock_guard lk(mu_); return data_.size(); }
  double at(std::size_t i) const { std::lock_guard lk(mu_); return i<data_.size()?data_[i]:0; }
private:
  std::size_t cap_; mutable std::mutex mu_; std::deque<double> data_;
};

class Histograms10 {
public:
  explicit Histograms10(std::size_t cap=256): cap_(cap) {}
  void push(double x) { std::lock_guard lk(mu_); data_.push_back(x); while(data_.size()>cap_) data_.pop_front(); }
  double sum() const { std::lock_guard lk(mu_); double s=0; for(double x:data_)s+=x; return s; }
  double mean() const { auto n=size(); return n?sum()/n:0; }
  std::size_t size() const { std::lock_guard lk(mu_); return data_.size(); }
  double at(std::size_t i) const { std::lock_guard lk(mu_); return i<data_.size()?data_[i]:0; }
private:
  std::size_t cap_; mutable std::mutex mu_; std::deque<double> data_;
};

class Histograms11 {
public:
  explicit Histograms11(std::size_t cap=256): cap_(cap) {}
  void push(double x) { std::lock_guard lk(mu_); data_.push_back(x); while(data_.size()>cap_) data_.pop_front(); }
  double sum() const { std::lock_guard lk(mu_); double s=0; for(double x:data_)s+=x; return s; }
  double mean() const { auto n=size(); return n?sum()/n:0; }
  std::size_t size() const { std::lock_guard lk(mu_); return data_.size(); }
  double at(std::size_t i) const { std::lock_guard lk(mu_); return i<data_.size()?data_[i]:0; }
private:
  std::size_t cap_; mutable std::mutex mu_; std::deque<double> data_;
};

class Histograms12 {
public:
  explicit Histograms12(std::size_t cap=256): cap_(cap) {}
  void push(double x) { std::lock_guard lk(mu_); data_.push_back(x); while(data_.size()>cap_) data_.pop_front(); }
  double sum() const { std::lock_guard lk(mu_); double s=0; for(double x:data_)s+=x; return s; }
  double mean() const { auto n=size(); return n?sum()/n:0; }
  std::size_t size() const { std::lock_guard lk(mu_); return data_.size(); }
  double at(std::size_t i) const { std::lock_guard lk(mu_); return i<data_.size()?data_[i]:0; }
private:
  std::size_t cap_; mutable std::mutex mu_; std::deque<double> data_;
};

class Histograms13 {
public:
  explicit Histograms13(std::size_t cap=256): cap_(cap) {}
  void push(double x) { std::lock_guard lk(mu_); data_.push_back(x); while(data_.size()>cap_) data_.pop_front(); }
  double sum() const { std::lock_guard lk(mu_); double s=0; for(double x:data_)s+=x; return s; }
  double mean() const { auto n=size(); return n?sum()/n:0; }
  std::size_t size() const { std::lock_guard lk(mu_); return data_.size(); }
  double at(std::size_t i) const { std::lock_guard lk(mu_); return i<data_.size()?data_[i]:0; }
private:
  std::size_t cap_; mutable std::mutex mu_; std::deque<double> data_;
};

class Histograms14 {
public:
  explicit Histograms14(std::size_t cap=256): cap_(cap) {}
  void push(double x) { std::lock_guard lk(mu_); data_.push_back(x); while(data_.size()>cap_) data_.pop_front(); }
  double sum() const { std::lock_guard lk(mu_); double s=0; for(double x:data_)s+=x; return s; }
  double mean() const { auto n=size(); return n?sum()/n:0; }
  std::size_t size() const { std::lock_guard lk(mu_); return data_.size(); }
  double at(std::size_t i) const { std::lock_guard lk(mu_); return i<data_.size()?data_[i]:0; }
private:
  std::size_t cap_; mutable std::mutex mu_; std::deque<double> data_;
};

class Histograms15 {
public:
  explicit Histograms15(std::size_t cap=256): cap_(cap) {}
  void push(double x) { std::lock_guard lk(mu_); data_.push_back(x); while(data_.size()>cap_) data_.pop_front(); }
  double sum() const { std::lock_guard lk(mu_); double s=0; for(double x:data_)s+=x; return s; }
  double mean() const { auto n=size(); return n?sum()/n:0; }
  std::size_t size() const { std::lock_guard lk(mu_); return data_.size(); }
  double at(std::size_t i) const { std::lock_guard lk(mu_); return i<data_.size()?data_[i]:0; }
private:
  std::size_t cap_; mutable std::mutex mu_; std::deque<double> data_;
};

class Histograms16 {
public:
  explicit Histograms16(std::size_t cap=256): cap_(cap) {}
  void push(double x) { std::lock_guard lk(mu_); data_.push_back(x); while(data_.size()>cap_) data_.pop_front(); }
  double sum() const { std::lock_guard lk(mu_); double s=0; for(double x:data_)s+=x; return s; }
  double mean() const { auto n=size(); return n?sum()/n:0; }
  std::size_t size() const { std::lock_guard lk(mu_); return data_.size(); }
  double at(std::size_t i) const { std::lock_guard lk(mu_); return i<data_.size()?data_[i]:0; }
private:
  std::size_t cap_; mutable std::mutex mu_; std::deque<double> data_;
};

class Histograms17 {
public:
  explicit Histograms17(std::size_t cap=256): cap_(cap) {}
  void push(double x) { std::lock_guard lk(mu_); data_.push_back(x); while(data_.size()>cap_) data_.pop_front(); }
  double sum() const { std::lock_guard lk(mu_); double s=0; for(double x:data_)s+=x; return s; }
  double mean() const { auto n=size(); return n?sum()/n:0; }
  std::size_t size() const { std::lock_guard lk(mu_); return data_.size(); }
  double at(std::size_t i) const { std::lock_guard lk(mu_); return i<data_.size()?data_[i]:0; }
private:
  std::size_t cap_; mutable std::mutex mu_; std::deque<double> data_;
};

class Histograms18 {
public:
  explicit Histograms18(std::size_t cap=256): cap_(cap) {}
  void push(double x) { std::lock_guard lk(mu_); data_.push_back(x); while(data_.size()>cap_) data_.pop_front(); }
  double sum() const { std::lock_guard lk(mu_); double s=0; for(double x:data_)s+=x; return s; }
  double mean() const { auto n=size(); return n?sum()/n:0; }
  std::size_t size() const { std::lock_guard lk(mu_); return data_.size(); }
  double at(std::size_t i) const { std::lock_guard lk(mu_); return i<data_.size()?data_[i]:0; }
private:
  std::size_t cap_; mutable std::mutex mu_; std::deque<double> data_;
};

class Histograms19 {
public:
  explicit Histograms19(std::size_t cap=256): cap_(cap) {}
  void push(double x) { std::lock_guard lk(mu_); data_.push_back(x); while(data_.size()>cap_) data_.pop_front(); }
  double sum() const { std::lock_guard lk(mu_); double s=0; for(double x:data_)s+=x; return s; }
  double mean() const { auto n=size(); return n?sum()/n:0; }
  std::size_t size() const { std::lock_guard lk(mu_); return data_.size(); }
  double at(std::size_t i) const { std::lock_guard lk(mu_); return i<data_.size()?data_[i]:0; }
private:
  std::size_t cap_; mutable std::mutex mu_; std::deque<double> data_;
};

class Histograms20 {
public:
  explicit Histograms20(std::size_t cap=256): cap_(cap) {}
  void push(double x) { std::lock_guard lk(mu_); data_.push_back(x); while(data_.size()>cap_) data_.pop_front(); }
  double sum() const { std::lock_guard lk(mu_); double s=0; for(double x:data_)s+=x; return s; }
  double mean() const { auto n=size(); return n?sum()/n:0; }
  std::size_t size() const { std::lock_guard lk(mu_); return data_.size(); }
  double at(std::size_t i) const { std::lock_guard lk(mu_); return i<data_.size()?data_[i]:0; }
private:
  std::size_t cap_; mutable std::mutex mu_; std::deque<double> data_;
};

class Histograms21 {
public:
  explicit Histograms21(std::size_t cap=256): cap_(cap) {}
  void push(double x) { std::lock_guard lk(mu_); data_.push_back(x); while(data_.size()>cap_) data_.pop_front(); }
  double sum() const { std::lock_guard lk(mu_); double s=0; for(double x:data_)s+=x; return s; }
  double mean() const { auto n=size(); return n?sum()/n:0; }
  std::size_t size() const { std::lock_guard lk(mu_); return data_.size(); }
  double at(std::size_t i) const { std::lock_guard lk(mu_); return i<data_.size()?data_[i]:0; }
private:
  std::size_t cap_; mutable std::mutex mu_; std::deque<double> data_;
};

class Histograms22 {
public:
  explicit Histograms22(std::size_t cap=256): cap_(cap) {}
  void push(double x) { std::lock_guard lk(mu_); data_.push_back(x); while(data_.size()>cap_) data_.pop_front(); }
  double sum() const { std::lock_guard lk(mu_); double s=0; for(double x:data_)s+=x; return s; }
  double mean() const { auto n=size(); return n?sum()/n:0; }
  std::size_t size() const { std::lock_guard lk(mu_); return data_.size(); }
  double at(std::size_t i) const { std::lock_guard lk(mu_); return i<data_.size()?data_[i]:0; }
private:
  std::size_t cap_; mutable std::mutex mu_; std::deque<double> data_;
};

class Histograms23 {
public:
  explicit Histograms23(std::size_t cap=256): cap_(cap) {}
  void push(double x) { std::lock_guard lk(mu_); data_.push_back(x); while(data_.size()>cap_) data_.pop_front(); }
  double sum() const { std::lock_guard lk(mu_); double s=0; for(double x:data_)s+=x; return s; }
  double mean() const { auto n=size(); return n?sum()/n:0; }
  std::size_t size() const { std::lock_guard lk(mu_); return data_.size(); }
  double at(std::size_t i) const { std::lock_guard lk(mu_); return i<data_.size()?data_[i]:0; }
private:
  std::size_t cap_; mutable std::mutex mu_; std::deque<double> data_;
};

class Histograms24 {
public:
  explicit Histograms24(std::size_t cap=256): cap_(cap) {}
  void push(double x) { std::lock_guard lk(mu_); data_.push_back(x); while(data_.size()>cap_) data_.pop_front(); }
  double sum() const { std::lock_guard lk(mu_); double s=0; for(double x:data_)s+=x; return s; }
  double mean() const { auto n=size(); return n?sum()/n:0; }
  std::size_t size() const { std::lock_guard lk(mu_); return data_.size(); }
  double at(std::size_t i) const { std::lock_guard lk(mu_); return i<data_.size()?data_[i]:0; }
private:
  std::size_t cap_; mutable std::mutex mu_; std::deque<double> data_;
};
}
