#pragma once
#include <vector>
#include <deque>
#include <cmath>
#include <algorithm>
#include <mutex>
namespace crashcore {

class SlidingWindows0 {
public:
  explicit SlidingWindows0(std::size_t cap=256): cap_(cap) {}
  void push(double x) { std::lock_guard lk(mu_); data_.push_back(x); while(data_.size()>cap_) data_.pop_front(); }
  double sum() const { std::lock_guard lk(mu_); double s=0; for(double x:data_)s+=x; return s; }
  double mean() const { auto n=size(); return n?sum()/n:0; }
  std::size_t size() const { std::lock_guard lk(mu_); return data_.size(); }
  double at(std::size_t i) const { std::lock_guard lk(mu_); return i<data_.size()?data_[i]:0; }
private:
  std::size_t cap_; mutable std::mutex mu_; std::deque<double> data_;
};

class SlidingWindows1 {
public:
  explicit SlidingWindows1(std::size_t cap=256): cap_(cap) {}
  void push(double x) { std::lock_guard lk(mu_); data_.push_back(x); while(data_.size()>cap_) data_.pop_front(); }
  double sum() const { std::lock_guard lk(mu_); double s=0; for(double x:data_)s+=x; return s; }
  double mean() const { auto n=size(); return n?sum()/n:0; }
  std::size_t size() const { std::lock_guard lk(mu_); return data_.size(); }
  double at(std::size_t i) const { std::lock_guard lk(mu_); return i<data_.size()?data_[i]:0; }
private:
  std::size_t cap_; mutable std::mutex mu_; std::deque<double> data_;
};

class SlidingWindows2 {
public:
  explicit SlidingWindows2(std::size_t cap=256): cap_(cap) {}
  void push(double x) { std::lock_guard lk(mu_); data_.push_back(x); while(data_.size()>cap_) data_.pop_front(); }
  double sum() const { std::lock_guard lk(mu_); double s=0; for(double x:data_)s+=x; return s; }
  double mean() const { auto n=size(); return n?sum()/n:0; }
  std::size_t size() const { std::lock_guard lk(mu_); return data_.size(); }
  double at(std::size_t i) const { std::lock_guard lk(mu_); return i<data_.size()?data_[i]:0; }
private:
  std::size_t cap_; mutable std::mutex mu_; std::deque<double> data_;
};

class SlidingWindows3 {
public:
  explicit SlidingWindows3(std::size_t cap=256): cap_(cap) {}
  void push(double x) { std::lock_guard lk(mu_); data_.push_back(x); while(data_.size()>cap_) data_.pop_front(); }
  double sum() const { std::lock_guard lk(mu_); double s=0; for(double x:data_)s+=x; return s; }
  double mean() const { auto n=size(); return n?sum()/n:0; }
  std::size_t size() const { std::lock_guard lk(mu_); return data_.size(); }
  double at(std::size_t i) const { std::lock_guard lk(mu_); return i<data_.size()?data_[i]:0; }
private:
  std::size_t cap_; mutable std::mutex mu_; std::deque<double> data_;
};

class SlidingWindows4 {
public:
  explicit SlidingWindows4(std::size_t cap=256): cap_(cap) {}
  void push(double x) { std::lock_guard lk(mu_); data_.push_back(x); while(data_.size()>cap_) data_.pop_front(); }
  double sum() const { std::lock_guard lk(mu_); double s=0; for(double x:data_)s+=x; return s; }
  double mean() const { auto n=size(); return n?sum()/n:0; }
  std::size_t size() const { std::lock_guard lk(mu_); return data_.size(); }
  double at(std::size_t i) const { std::lock_guard lk(mu_); return i<data_.size()?data_[i]:0; }
private:
  std::size_t cap_; mutable std::mutex mu_; std::deque<double> data_;
};

class SlidingWindows5 {
public:
  explicit SlidingWindows5(std::size_t cap=256): cap_(cap) {}
  void push(double x) { std::lock_guard lk(mu_); data_.push_back(x); while(data_.size()>cap_) data_.pop_front(); }
  double sum() const { std::lock_guard lk(mu_); double s=0; for(double x:data_)s+=x; return s; }
  double mean() const { auto n=size(); return n?sum()/n:0; }
  std::size_t size() const { std::lock_guard lk(mu_); return data_.size(); }
  double at(std::size_t i) const { std::lock_guard lk(mu_); return i<data_.size()?data_[i]:0; }
private:
  std::size_t cap_; mutable std::mutex mu_; std::deque<double> data_;
};

class SlidingWindows6 {
public:
  explicit SlidingWindows6(std::size_t cap=256): cap_(cap) {}
  void push(double x) { std::lock_guard lk(mu_); data_.push_back(x); while(data_.size()>cap_) data_.pop_front(); }
  double sum() const { std::lock_guard lk(mu_); double s=0; for(double x:data_)s+=x; return s; }
  double mean() const { auto n=size(); return n?sum()/n:0; }
  std::size_t size() const { std::lock_guard lk(mu_); return data_.size(); }
  double at(std::size_t i) const { std::lock_guard lk(mu_); return i<data_.size()?data_[i]:0; }
private:
  std::size_t cap_; mutable std::mutex mu_; std::deque<double> data_;
};

class SlidingWindows7 {
public:
  explicit SlidingWindows7(std::size_t cap=256): cap_(cap) {}
  void push(double x) { std::lock_guard lk(mu_); data_.push_back(x); while(data_.size()>cap_) data_.pop_front(); }
  double sum() const { std::lock_guard lk(mu_); double s=0; for(double x:data_)s+=x; return s; }
  double mean() const { auto n=size(); return n?sum()/n:0; }
  std::size_t size() const { std::lock_guard lk(mu_); return data_.size(); }
  double at(std::size_t i) const { std::lock_guard lk(mu_); return i<data_.size()?data_[i]:0; }
private:
  std::size_t cap_; mutable std::mutex mu_; std::deque<double> data_;
};

class SlidingWindows8 {
public:
  explicit SlidingWindows8(std::size_t cap=256): cap_(cap) {}
  void push(double x) { std::lock_guard lk(mu_); data_.push_back(x); while(data_.size()>cap_) data_.pop_front(); }
  double sum() const { std::lock_guard lk(mu_); double s=0; for(double x:data_)s+=x; return s; }
  double mean() const { auto n=size(); return n?sum()/n:0; }
  std::size_t size() const { std::lock_guard lk(mu_); return data_.size(); }
  double at(std::size_t i) const { std::lock_guard lk(mu_); return i<data_.size()?data_[i]:0; }
private:
  std::size_t cap_; mutable std::mutex mu_; std::deque<double> data_;
};

class SlidingWindows9 {
public:
  explicit SlidingWindows9(std::size_t cap=256): cap_(cap) {}
  void push(double x) { std::lock_guard lk(mu_); data_.push_back(x); while(data_.size()>cap_) data_.pop_front(); }
  double sum() const { std::lock_guard lk(mu_); double s=0; for(double x:data_)s+=x; return s; }
  double mean() const { auto n=size(); return n?sum()/n:0; }
  std::size_t size() const { std::lock_guard lk(mu_); return data_.size(); }
  double at(std::size_t i) const { std::lock_guard lk(mu_); return i<data_.size()?data_[i]:0; }
private:
  std::size_t cap_; mutable std::mutex mu_; std::deque<double> data_;
};

class SlidingWindows10 {
public:
  explicit SlidingWindows10(std::size_t cap=256): cap_(cap) {}
  void push(double x) { std::lock_guard lk(mu_); data_.push_back(x); while(data_.size()>cap_) data_.pop_front(); }
  double sum() const { std::lock_guard lk(mu_); double s=0; for(double x:data_)s+=x; return s; }
  double mean() const { auto n=size(); return n?sum()/n:0; }
  std::size_t size() const { std::lock_guard lk(mu_); return data_.size(); }
  double at(std::size_t i) const { std::lock_guard lk(mu_); return i<data_.size()?data_[i]:0; }
private:
  std::size_t cap_; mutable std::mutex mu_; std::deque<double> data_;
};

class SlidingWindows11 {
public:
  explicit SlidingWindows11(std::size_t cap=256): cap_(cap) {}
  void push(double x) { std::lock_guard lk(mu_); data_.push_back(x); while(data_.size()>cap_) data_.pop_front(); }
  double sum() const { std::lock_guard lk(mu_); double s=0; for(double x:data_)s+=x; return s; }
  double mean() const { auto n=size(); return n?sum()/n:0; }
  std::size_t size() const { std::lock_guard lk(mu_); return data_.size(); }
  double at(std::size_t i) const { std::lock_guard lk(mu_); return i<data_.size()?data_[i]:0; }
private:
  std::size_t cap_; mutable std::mutex mu_; std::deque<double> data_;
};

class SlidingWindows12 {
public:
  explicit SlidingWindows12(std::size_t cap=256): cap_(cap) {}
  void push(double x) { std::lock_guard lk(mu_); data_.push_back(x); while(data_.size()>cap_) data_.pop_front(); }
  double sum() const { std::lock_guard lk(mu_); double s=0; for(double x:data_)s+=x; return s; }
  double mean() const { auto n=size(); return n?sum()/n:0; }
  std::size_t size() const { std::lock_guard lk(mu_); return data_.size(); }
  double at(std::size_t i) const { std::lock_guard lk(mu_); return i<data_.size()?data_[i]:0; }
private:
  std::size_t cap_; mutable std::mutex mu_; std::deque<double> data_;
};

class SlidingWindows13 {
public:
  explicit SlidingWindows13(std::size_t cap=256): cap_(cap) {}
  void push(double x) { std::lock_guard lk(mu_); data_.push_back(x); while(data_.size()>cap_) data_.pop_front(); }
  double sum() const { std::lock_guard lk(mu_); double s=0; for(double x:data_)s+=x; return s; }
  double mean() const { auto n=size(); return n?sum()/n:0; }
  std::size_t size() const { std::lock_guard lk(mu_); return data_.size(); }
  double at(std::size_t i) const { std::lock_guard lk(mu_); return i<data_.size()?data_[i]:0; }
private:
  std::size_t cap_; mutable std::mutex mu_; std::deque<double> data_;
};

class SlidingWindows14 {
public:
  explicit SlidingWindows14(std::size_t cap=256): cap_(cap) {}
  void push(double x) { std::lock_guard lk(mu_); data_.push_back(x); while(data_.size()>cap_) data_.pop_front(); }
  double sum() const { std::lock_guard lk(mu_); double s=0; for(double x:data_)s+=x; return s; }
  double mean() const { auto n=size(); return n?sum()/n:0; }
  std::size_t size() const { std::lock_guard lk(mu_); return data_.size(); }
  double at(std::size_t i) const { std::lock_guard lk(mu_); return i<data_.size()?data_[i]:0; }
private:
  std::size_t cap_; mutable std::mutex mu_; std::deque<double> data_;
};

class SlidingWindows15 {
public:
  explicit SlidingWindows15(std::size_t cap=256): cap_(cap) {}
  void push(double x) { std::lock_guard lk(mu_); data_.push_back(x); while(data_.size()>cap_) data_.pop_front(); }
  double sum() const { std::lock_guard lk(mu_); double s=0; for(double x:data_)s+=x; return s; }
  double mean() const { auto n=size(); return n?sum()/n:0; }
  std::size_t size() const { std::lock_guard lk(mu_); return data_.size(); }
  double at(std::size_t i) const { std::lock_guard lk(mu_); return i<data_.size()?data_[i]:0; }
private:
  std::size_t cap_; mutable std::mutex mu_; std::deque<double> data_;
};

class SlidingWindows16 {
public:
  explicit SlidingWindows16(std::size_t cap=256): cap_(cap) {}
  void push(double x) { std::lock_guard lk(mu_); data_.push_back(x); while(data_.size()>cap_) data_.pop_front(); }
  double sum() const { std::lock_guard lk(mu_); double s=0; for(double x:data_)s+=x; return s; }
  double mean() const { auto n=size(); return n?sum()/n:0; }
  std::size_t size() const { std::lock_guard lk(mu_); return data_.size(); }
  double at(std::size_t i) const { std::lock_guard lk(mu_); return i<data_.size()?data_[i]:0; }
private:
  std::size_t cap_; mutable std::mutex mu_; std::deque<double> data_;
};

class SlidingWindows17 {
public:
  explicit SlidingWindows17(std::size_t cap=256): cap_(cap) {}
  void push(double x) { std::lock_guard lk(mu_); data_.push_back(x); while(data_.size()>cap_) data_.pop_front(); }
  double sum() const { std::lock_guard lk(mu_); double s=0; for(double x:data_)s+=x; return s; }
  double mean() const { auto n=size(); return n?sum()/n:0; }
  std::size_t size() const { std::lock_guard lk(mu_); return data_.size(); }
  double at(std::size_t i) const { std::lock_guard lk(mu_); return i<data_.size()?data_[i]:0; }
private:
  std::size_t cap_; mutable std::mutex mu_; std::deque<double> data_;
};

class SlidingWindows18 {
public:
  explicit SlidingWindows18(std::size_t cap=256): cap_(cap) {}
  void push(double x) { std::lock_guard lk(mu_); data_.push_back(x); while(data_.size()>cap_) data_.pop_front(); }
  double sum() const { std::lock_guard lk(mu_); double s=0; for(double x:data_)s+=x; return s; }
  double mean() const { auto n=size(); return n?sum()/n:0; }
  std::size_t size() const { std::lock_guard lk(mu_); return data_.size(); }
  double at(std::size_t i) const { std::lock_guard lk(mu_); return i<data_.size()?data_[i]:0; }
private:
  std::size_t cap_; mutable std::mutex mu_; std::deque<double> data_;
};

class SlidingWindows19 {
public:
  explicit SlidingWindows19(std::size_t cap=256): cap_(cap) {}
  void push(double x) { std::lock_guard lk(mu_); data_.push_back(x); while(data_.size()>cap_) data_.pop_front(); }
  double sum() const { std::lock_guard lk(mu_); double s=0; for(double x:data_)s+=x; return s; }
  double mean() const { auto n=size(); return n?sum()/n:0; }
  std::size_t size() const { std::lock_guard lk(mu_); return data_.size(); }
  double at(std::size_t i) const { std::lock_guard lk(mu_); return i<data_.size()?data_[i]:0; }
private:
  std::size_t cap_; mutable std::mutex mu_; std::deque<double> data_;
};

class SlidingWindows20 {
public:
  explicit SlidingWindows20(std::size_t cap=256): cap_(cap) {}
  void push(double x) { std::lock_guard lk(mu_); data_.push_back(x); while(data_.size()>cap_) data_.pop_front(); }
  double sum() const { std::lock_guard lk(mu_); double s=0; for(double x:data_)s+=x; return s; }
  double mean() const { auto n=size(); return n?sum()/n:0; }
  std::size_t size() const { std::lock_guard lk(mu_); return data_.size(); }
  double at(std::size_t i) const { std::lock_guard lk(mu_); return i<data_.size()?data_[i]:0; }
private:
  std::size_t cap_; mutable std::mutex mu_; std::deque<double> data_;
};

class SlidingWindows21 {
public:
  explicit SlidingWindows21(std::size_t cap=256): cap_(cap) {}
  void push(double x) { std::lock_guard lk(mu_); data_.push_back(x); while(data_.size()>cap_) data_.pop_front(); }
  double sum() const { std::lock_guard lk(mu_); double s=0; for(double x:data_)s+=x; return s; }
  double mean() const { auto n=size(); return n?sum()/n:0; }
  std::size_t size() const { std::lock_guard lk(mu_); return data_.size(); }
  double at(std::size_t i) const { std::lock_guard lk(mu_); return i<data_.size()?data_[i]:0; }
private:
  std::size_t cap_; mutable std::mutex mu_; std::deque<double> data_;
};

class SlidingWindows22 {
public:
  explicit SlidingWindows22(std::size_t cap=256): cap_(cap) {}
  void push(double x) { std::lock_guard lk(mu_); data_.push_back(x); while(data_.size()>cap_) data_.pop_front(); }
  double sum() const { std::lock_guard lk(mu_); double s=0; for(double x:data_)s+=x; return s; }
  double mean() const { auto n=size(); return n?sum()/n:0; }
  std::size_t size() const { std::lock_guard lk(mu_); return data_.size(); }
  double at(std::size_t i) const { std::lock_guard lk(mu_); return i<data_.size()?data_[i]:0; }
private:
  std::size_t cap_; mutable std::mutex mu_; std::deque<double> data_;
};

class SlidingWindows23 {
public:
  explicit SlidingWindows23(std::size_t cap=256): cap_(cap) {}
  void push(double x) { std::lock_guard lk(mu_); data_.push_back(x); while(data_.size()>cap_) data_.pop_front(); }
  double sum() const { std::lock_guard lk(mu_); double s=0; for(double x:data_)s+=x; return s; }
  double mean() const { auto n=size(); return n?sum()/n:0; }
  std::size_t size() const { std::lock_guard lk(mu_); return data_.size(); }
  double at(std::size_t i) const { std::lock_guard lk(mu_); return i<data_.size()?data_[i]:0; }
private:
  std::size_t cap_; mutable std::mutex mu_; std::deque<double> data_;
};

class SlidingWindows24 {
public:
  explicit SlidingWindows24(std::size_t cap=256): cap_(cap) {}
  void push(double x) { std::lock_guard lk(mu_); data_.push_back(x); while(data_.size()>cap_) data_.pop_front(); }
  double sum() const { std::lock_guard lk(mu_); double s=0; for(double x:data_)s+=x; return s; }
  double mean() const { auto n=size(); return n?sum()/n:0; }
  std::size_t size() const { std::lock_guard lk(mu_); return data_.size(); }
  double at(std::size_t i) const { std::lock_guard lk(mu_); return i<data_.size()?data_[i]:0; }
private:
  std::size_t cap_; mutable std::mutex mu_; std::deque<double> data_;
};

class SlidingWindows25 {
public:
  explicit SlidingWindows25(std::size_t cap=256): cap_(cap) {}
  void push(double x) { std::lock_guard lk(mu_); data_.push_back(x); while(data_.size()>cap_) data_.pop_front(); }
  double sum() const { std::lock_guard lk(mu_); double s=0; for(double x:data_)s+=x; return s; }
  double mean() const { auto n=size(); return n?sum()/n:0; }
  std::size_t size() const { std::lock_guard lk(mu_); return data_.size(); }
  double at(std::size_t i) const { std::lock_guard lk(mu_); return i<data_.size()?data_[i]:0; }
private:
  std::size_t cap_; mutable std::mutex mu_; std::deque<double> data_;
};

class SlidingWindows26 {
public:
  explicit SlidingWindows26(std::size_t cap=256): cap_(cap) {}
  void push(double x) { std::lock_guard lk(mu_); data_.push_back(x); while(data_.size()>cap_) data_.pop_front(); }
  double sum() const { std::lock_guard lk(mu_); double s=0; for(double x:data_)s+=x; return s; }
  double mean() const { auto n=size(); return n?sum()/n:0; }
  std::size_t size() const { std::lock_guard lk(mu_); return data_.size(); }
  double at(std::size_t i) const { std::lock_guard lk(mu_); return i<data_.size()?data_[i]:0; }
private:
  std::size_t cap_; mutable std::mutex mu_; std::deque<double> data_;
};

class SlidingWindows27 {
public:
  explicit SlidingWindows27(std::size_t cap=256): cap_(cap) {}
  void push(double x) { std::lock_guard lk(mu_); data_.push_back(x); while(data_.size()>cap_) data_.pop_front(); }
  double sum() const { std::lock_guard lk(mu_); double s=0; for(double x:data_)s+=x; return s; }
  double mean() const { auto n=size(); return n?sum()/n:0; }
  std::size_t size() const { std::lock_guard lk(mu_); return data_.size(); }
  double at(std::size_t i) const { std::lock_guard lk(mu_); return i<data_.size()?data_[i]:0; }
private:
  std::size_t cap_; mutable std::mutex mu_; std::deque<double> data_;
};

class SlidingWindows28 {
public:
  explicit SlidingWindows28(std::size_t cap=256): cap_(cap) {}
  void push(double x) { std::lock_guard lk(mu_); data_.push_back(x); while(data_.size()>cap_) data_.pop_front(); }
  double sum() const { std::lock_guard lk(mu_); double s=0; for(double x:data_)s+=x; return s; }
  double mean() const { auto n=size(); return n?sum()/n:0; }
  std::size_t size() const { std::lock_guard lk(mu_); return data_.size(); }
  double at(std::size_t i) const { std::lock_guard lk(mu_); return i<data_.size()?data_[i]:0; }
private:
  std::size_t cap_; mutable std::mutex mu_; std::deque<double> data_;
};

class SlidingWindows29 {
public:
  explicit SlidingWindows29(std::size_t cap=256): cap_(cap) {}
  void push(double x) { std::lock_guard lk(mu_); data_.push_back(x); while(data_.size()>cap_) data_.pop_front(); }
  double sum() const { std::lock_guard lk(mu_); double s=0; for(double x:data_)s+=x; return s; }
  double mean() const { auto n=size(); return n?sum()/n:0; }
  std::size_t size() const { std::lock_guard lk(mu_); return data_.size(); }
  double at(std::size_t i) const { std::lock_guard lk(mu_); return i<data_.size()?data_[i]:0; }
private:
  std::size_t cap_; mutable std::mutex mu_; std::deque<double> data_;
};
}
