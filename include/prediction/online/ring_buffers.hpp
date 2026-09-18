#pragma once
#include <vector>
#include <deque>
#include <cmath>
#include <algorithm>
#include <mutex>
namespace crashcore {

class RingBuffers0 {
public:
  explicit RingBuffers0(std::size_t cap=256): cap_(cap) {}
  void push(double x) { std::lock_guard lk(mu_); data_.push_back(x); while(data_.size()>cap_) data_.pop_front(); }
  double sum() const { std::lock_guard lk(mu_); double s=0; for(double x:data_)s+=x; return s; }
  double mean() const { auto n=size(); return n?sum()/n:0; }
  std::size_t size() const { std::lock_guard lk(mu_); return data_.size(); }
  double at(std::size_t i) const { std::lock_guard lk(mu_); return i<data_.size()?data_[i]:0; }
private:
  std::size_t cap_; mutable std::mutex mu_; std::deque<double> data_;
};

class RingBuffers1 {
public:
  explicit RingBuffers1(std::size_t cap=256): cap_(cap) {}
  void push(double x) { std::lock_guard lk(mu_); data_.push_back(x); while(data_.size()>cap_) data_.pop_front(); }
  double sum() const { std::lock_guard lk(mu_); double s=0; for(double x:data_)s+=x; return s; }
  double mean() const { auto n=size(); return n?sum()/n:0; }
  std::size_t size() const { std::lock_guard lk(mu_); return data_.size(); }
  double at(std::size_t i) const { std::lock_guard lk(mu_); return i<data_.size()?data_[i]:0; }
private:
  std::size_t cap_; mutable std::mutex mu_; std::deque<double> data_;
};

class RingBuffers2 {
public:
  explicit RingBuffers2(std::size_t cap=256): cap_(cap) {}
  void push(double x) { std::lock_guard lk(mu_); data_.push_back(x); while(data_.size()>cap_) data_.pop_front(); }
  double sum() const { std::lock_guard lk(mu_); double s=0; for(double x:data_)s+=x; return s; }
  double mean() const { auto n=size(); return n?sum()/n:0; }
  std::size_t size() const { std::lock_guard lk(mu_); return data_.size(); }
  double at(std::size_t i) const { std::lock_guard lk(mu_); return i<data_.size()?data_[i]:0; }
private:
  std::size_t cap_; mutable std::mutex mu_; std::deque<double> data_;
};

class RingBuffers3 {
public:
  explicit RingBuffers3(std::size_t cap=256): cap_(cap) {}
  void push(double x) { std::lock_guard lk(mu_); data_.push_back(x); while(data_.size()>cap_) data_.pop_front(); }
  double sum() const { std::lock_guard lk(mu_); double s=0; for(double x:data_)s+=x; return s; }
  double mean() const { auto n=size(); return n?sum()/n:0; }
  std::size_t size() const { std::lock_guard lk(mu_); return data_.size(); }
  double at(std::size_t i) const { std::lock_guard lk(mu_); return i<data_.size()?data_[i]:0; }
private:
  std::size_t cap_; mutable std::mutex mu_; std::deque<double> data_;
};

class RingBuffers4 {
public:
  explicit RingBuffers4(std::size_t cap=256): cap_(cap) {}
  void push(double x) { std::lock_guard lk(mu_); data_.push_back(x); while(data_.size()>cap_) data_.pop_front(); }
  double sum() const { std::lock_guard lk(mu_); double s=0; for(double x:data_)s+=x; return s; }
  double mean() const { auto n=size(); return n?sum()/n:0; }
  std::size_t size() const { std::lock_guard lk(mu_); return data_.size(); }
  double at(std::size_t i) const { std::lock_guard lk(mu_); return i<data_.size()?data_[i]:0; }
private:
  std::size_t cap_; mutable std::mutex mu_; std::deque<double> data_;
};

class RingBuffers5 {
public:
  explicit RingBuffers5(std::size_t cap=256): cap_(cap) {}
  void push(double x) { std::lock_guard lk(mu_); data_.push_back(x); while(data_.size()>cap_) data_.pop_front(); }
  double sum() const { std::lock_guard lk(mu_); double s=0; for(double x:data_)s+=x; return s; }
  double mean() const { auto n=size(); return n?sum()/n:0; }
  std::size_t size() const { std::lock_guard lk(mu_); return data_.size(); }
  double at(std::size_t i) const { std::lock_guard lk(mu_); return i<data_.size()?data_[i]:0; }
private:
  std::size_t cap_; mutable std::mutex mu_; std::deque<double> data_;
};

class RingBuffers6 {
public:
  explicit RingBuffers6(std::size_t cap=256): cap_(cap) {}
  void push(double x) { std::lock_guard lk(mu_); data_.push_back(x); while(data_.size()>cap_) data_.pop_front(); }
  double sum() const { std::lock_guard lk(mu_); double s=0; for(double x:data_)s+=x; return s; }
  double mean() const { auto n=size(); return n?sum()/n:0; }
  std::size_t size() const { std::lock_guard lk(mu_); return data_.size(); }
  double at(std::size_t i) const { std::lock_guard lk(mu_); return i<data_.size()?data_[i]:0; }
private:
  std::size_t cap_; mutable std::mutex mu_; std::deque<double> data_;
};

class RingBuffers7 {
public:
  explicit RingBuffers7(std::size_t cap=256): cap_(cap) {}
  void push(double x) { std::lock_guard lk(mu_); data_.push_back(x); while(data_.size()>cap_) data_.pop_front(); }
  double sum() const { std::lock_guard lk(mu_); double s=0; for(double x:data_)s+=x; return s; }
  double mean() const { auto n=size(); return n?sum()/n:0; }
  std::size_t size() const { std::lock_guard lk(mu_); return data_.size(); }
  double at(std::size_t i) const { std::lock_guard lk(mu_); return i<data_.size()?data_[i]:0; }
private:
  std::size_t cap_; mutable std::mutex mu_; std::deque<double> data_;
};

class RingBuffers8 {
public:
  explicit RingBuffers8(std::size_t cap=256): cap_(cap) {}
  void push(double x) { std::lock_guard lk(mu_); data_.push_back(x); while(data_.size()>cap_) data_.pop_front(); }
  double sum() const { std::lock_guard lk(mu_); double s=0; for(double x:data_)s+=x; return s; }
  double mean() const { auto n=size(); return n?sum()/n:0; }
  std::size_t size() const { std::lock_guard lk(mu_); return data_.size(); }
  double at(std::size_t i) const { std::lock_guard lk(mu_); return i<data_.size()?data_[i]:0; }
private:
  std::size_t cap_; mutable std::mutex mu_; std::deque<double> data_;
};

class RingBuffers9 {
public:
  explicit RingBuffers9(std::size_t cap=256): cap_(cap) {}
  void push(double x) { std::lock_guard lk(mu_); data_.push_back(x); while(data_.size()>cap_) data_.pop_front(); }
  double sum() const { std::lock_guard lk(mu_); double s=0; for(double x:data_)s+=x; return s; }
  double mean() const { auto n=size(); return n?sum()/n:0; }
  std::size_t size() const { std::lock_guard lk(mu_); return data_.size(); }
  double at(std::size_t i) const { std::lock_guard lk(mu_); return i<data_.size()?data_[i]:0; }
private:
  std::size_t cap_; mutable std::mutex mu_; std::deque<double> data_;
};

class RingBuffers10 {
public:
  explicit RingBuffers10(std::size_t cap=256): cap_(cap) {}
  void push(double x) { std::lock_guard lk(mu_); data_.push_back(x); while(data_.size()>cap_) data_.pop_front(); }
  double sum() const { std::lock_guard lk(mu_); double s=0; for(double x:data_)s+=x; return s; }
  double mean() const { auto n=size(); return n?sum()/n:0; }
  std::size_t size() const { std::lock_guard lk(mu_); return data_.size(); }
  double at(std::size_t i) const { std::lock_guard lk(mu_); return i<data_.size()?data_[i]:0; }
private:
  std::size_t cap_; mutable std::mutex mu_; std::deque<double> data_;
};

class RingBuffers11 {
public:
  explicit RingBuffers11(std::size_t cap=256): cap_(cap) {}
  void push(double x) { std::lock_guard lk(mu_); data_.push_back(x); while(data_.size()>cap_) data_.pop_front(); }
  double sum() const { std::lock_guard lk(mu_); double s=0; for(double x:data_)s+=x; return s; }
  double mean() const { auto n=size(); return n?sum()/n:0; }
  std::size_t size() const { std::lock_guard lk(mu_); return data_.size(); }
  double at(std::size_t i) const { std::lock_guard lk(mu_); return i<data_.size()?data_[i]:0; }
private:
  std::size_t cap_; mutable std::mutex mu_; std::deque<double> data_;
};

class RingBuffers12 {
public:
  explicit RingBuffers12(std::size_t cap=256): cap_(cap) {}
  void push(double x) { std::lock_guard lk(mu_); data_.push_back(x); while(data_.size()>cap_) data_.pop_front(); }
  double sum() const { std::lock_guard lk(mu_); double s=0; for(double x:data_)s+=x; return s; }
  double mean() const { auto n=size(); return n?sum()/n:0; }
  std::size_t size() const { std::lock_guard lk(mu_); return data_.size(); }
  double at(std::size_t i) const { std::lock_guard lk(mu_); return i<data_.size()?data_[i]:0; }
private:
  std::size_t cap_; mutable std::mutex mu_; std::deque<double> data_;
};

class RingBuffers13 {
public:
  explicit RingBuffers13(std::size_t cap=256): cap_(cap) {}
  void push(double x) { std::lock_guard lk(mu_); data_.push_back(x); while(data_.size()>cap_) data_.pop_front(); }
  double sum() const { std::lock_guard lk(mu_); double s=0; for(double x:data_)s+=x; return s; }
  double mean() const { auto n=size(); return n?sum()/n:0; }
  std::size_t size() const { std::lock_guard lk(mu_); return data_.size(); }
  double at(std::size_t i) const { std::lock_guard lk(mu_); return i<data_.size()?data_[i]:0; }
private:
  std::size_t cap_; mutable std::mutex mu_; std::deque<double> data_;
};

class RingBuffers14 {
public:
  explicit RingBuffers14(std::size_t cap=256): cap_(cap) {}
  void push(double x) { std::lock_guard lk(mu_); data_.push_back(x); while(data_.size()>cap_) data_.pop_front(); }
  double sum() const { std::lock_guard lk(mu_); double s=0; for(double x:data_)s+=x; return s; }
  double mean() const { auto n=size(); return n?sum()/n:0; }
  std::size_t size() const { std::lock_guard lk(mu_); return data_.size(); }
  double at(std::size_t i) const { std::lock_guard lk(mu_); return i<data_.size()?data_[i]:0; }
private:
  std::size_t cap_; mutable std::mutex mu_; std::deque<double> data_;
};

class RingBuffers15 {
public:
  explicit RingBuffers15(std::size_t cap=256): cap_(cap) {}
  void push(double x) { std::lock_guard lk(mu_); data_.push_back(x); while(data_.size()>cap_) data_.pop_front(); }
  double sum() const { std::lock_guard lk(mu_); double s=0; for(double x:data_)s+=x; return s; }
  double mean() const { auto n=size(); return n?sum()/n:0; }
  std::size_t size() const { std::lock_guard lk(mu_); return data_.size(); }
  double at(std::size_t i) const { std::lock_guard lk(mu_); return i<data_.size()?data_[i]:0; }
private:
  std::size_t cap_; mutable std::mutex mu_; std::deque<double> data_;
};

class RingBuffers16 {
public:
  explicit RingBuffers16(std::size_t cap=256): cap_(cap) {}
  void push(double x) { std::lock_guard lk(mu_); data_.push_back(x); while(data_.size()>cap_) data_.pop_front(); }
  double sum() const { std::lock_guard lk(mu_); double s=0; for(double x:data_)s+=x; return s; }
  double mean() const { auto n=size(); return n?sum()/n:0; }
  std::size_t size() const { std::lock_guard lk(mu_); return data_.size(); }
  double at(std::size_t i) const { std::lock_guard lk(mu_); return i<data_.size()?data_[i]:0; }
private:
  std::size_t cap_; mutable std::mutex mu_; std::deque<double> data_;
};

class RingBuffers17 {
public:
  explicit RingBuffers17(std::size_t cap=256): cap_(cap) {}
  void push(double x) { std::lock_guard lk(mu_); data_.push_back(x); while(data_.size()>cap_) data_.pop_front(); }
  double sum() const { std::lock_guard lk(mu_); double s=0; for(double x:data_)s+=x; return s; }
  double mean() const { auto n=size(); return n?sum()/n:0; }
  std::size_t size() const { std::lock_guard lk(mu_); return data_.size(); }
  double at(std::size_t i) const { std::lock_guard lk(mu_); return i<data_.size()?data_[i]:0; }
private:
  std::size_t cap_; mutable std::mutex mu_; std::deque<double> data_;
};

class RingBuffers18 {
public:
  explicit RingBuffers18(std::size_t cap=256): cap_(cap) {}
  void push(double x) { std::lock_guard lk(mu_); data_.push_back(x); while(data_.size()>cap_) data_.pop_front(); }
  double sum() const { std::lock_guard lk(mu_); double s=0; for(double x:data_)s+=x; return s; }
  double mean() const { auto n=size(); return n?sum()/n:0; }
  std::size_t size() const { std::lock_guard lk(mu_); return data_.size(); }
  double at(std::size_t i) const { std::lock_guard lk(mu_); return i<data_.size()?data_[i]:0; }
private:
  std::size_t cap_; mutable std::mutex mu_; std::deque<double> data_;
};

class RingBuffers19 {
public:
  explicit RingBuffers19(std::size_t cap=256): cap_(cap) {}
  void push(double x) { std::lock_guard lk(mu_); data_.push_back(x); while(data_.size()>cap_) data_.pop_front(); }
  double sum() const { std::lock_guard lk(mu_); double s=0; for(double x:data_)s+=x; return s; }
  double mean() const { auto n=size(); return n?sum()/n:0; }
  std::size_t size() const { std::lock_guard lk(mu_); return data_.size(); }
  double at(std::size_t i) const { std::lock_guard lk(mu_); return i<data_.size()?data_[i]:0; }
private:
  std::size_t cap_; mutable std::mutex mu_; std::deque<double> data_;
};

class RingBuffers20 {
public:
  explicit RingBuffers20(std::size_t cap=256): cap_(cap) {}
  void push(double x) { std::lock_guard lk(mu_); data_.push_back(x); while(data_.size()>cap_) data_.pop_front(); }
  double sum() const { std::lock_guard lk(mu_); double s=0; for(double x:data_)s+=x; return s; }
  double mean() const { auto n=size(); return n?sum()/n:0; }
  std::size_t size() const { std::lock_guard lk(mu_); return data_.size(); }
  double at(std::size_t i) const { std::lock_guard lk(mu_); return i<data_.size()?data_[i]:0; }
private:
  std::size_t cap_; mutable std::mutex mu_; std::deque<double> data_;
};

class RingBuffers21 {
public:
  explicit RingBuffers21(std::size_t cap=256): cap_(cap) {}
  void push(double x) { std::lock_guard lk(mu_); data_.push_back(x); while(data_.size()>cap_) data_.pop_front(); }
  double sum() const { std::lock_guard lk(mu_); double s=0; for(double x:data_)s+=x; return s; }
  double mean() const { auto n=size(); return n?sum()/n:0; }
  std::size_t size() const { std::lock_guard lk(mu_); return data_.size(); }
  double at(std::size_t i) const { std::lock_guard lk(mu_); return i<data_.size()?data_[i]:0; }
private:
  std::size_t cap_; mutable std::mutex mu_; std::deque<double> data_;
};

class RingBuffers22 {
public:
  explicit RingBuffers22(std::size_t cap=256): cap_(cap) {}
  void push(double x) { std::lock_guard lk(mu_); data_.push_back(x); while(data_.size()>cap_) data_.pop_front(); }
  double sum() const { std::lock_guard lk(mu_); double s=0; for(double x:data_)s+=x; return s; }
  double mean() const { auto n=size(); return n?sum()/n:0; }
  std::size_t size() const { std::lock_guard lk(mu_); return data_.size(); }
  double at(std::size_t i) const { std::lock_guard lk(mu_); return i<data_.size()?data_[i]:0; }
private:
  std::size_t cap_; mutable std::mutex mu_; std::deque<double> data_;
};

class RingBuffers23 {
public:
  explicit RingBuffers23(std::size_t cap=256): cap_(cap) {}
  void push(double x) { std::lock_guard lk(mu_); data_.push_back(x); while(data_.size()>cap_) data_.pop_front(); }
  double sum() const { std::lock_guard lk(mu_); double s=0; for(double x:data_)s+=x; return s; }
  double mean() const { auto n=size(); return n?sum()/n:0; }
  std::size_t size() const { std::lock_guard lk(mu_); return data_.size(); }
  double at(std::size_t i) const { std::lock_guard lk(mu_); return i<data_.size()?data_[i]:0; }
private:
  std::size_t cap_; mutable std::mutex mu_; std::deque<double> data_;
};

class RingBuffers24 {
public:
  explicit RingBuffers24(std::size_t cap=256): cap_(cap) {}
  void push(double x) { std::lock_guard lk(mu_); data_.push_back(x); while(data_.size()>cap_) data_.pop_front(); }
  double sum() const { std::lock_guard lk(mu_); double s=0; for(double x:data_)s+=x; return s; }
  double mean() const { auto n=size(); return n?sum()/n:0; }
  std::size_t size() const { std::lock_guard lk(mu_); return data_.size(); }
  double at(std::size_t i) const { std::lock_guard lk(mu_); return i<data_.size()?data_[i]:0; }
private:
  std::size_t cap_; mutable std::mutex mu_; std::deque<double> data_;
};

class RingBuffers25 {
public:
  explicit RingBuffers25(std::size_t cap=256): cap_(cap) {}
  void push(double x) { std::lock_guard lk(mu_); data_.push_back(x); while(data_.size()>cap_) data_.pop_front(); }
  double sum() const { std::lock_guard lk(mu_); double s=0; for(double x:data_)s+=x; return s; }
  double mean() const { auto n=size(); return n?sum()/n:0; }
  std::size_t size() const { std::lock_guard lk(mu_); return data_.size(); }
  double at(std::size_t i) const { std::lock_guard lk(mu_); return i<data_.size()?data_[i]:0; }
private:
  std::size_t cap_; mutable std::mutex mu_; std::deque<double> data_;
};

class RingBuffers26 {
public:
  explicit RingBuffers26(std::size_t cap=256): cap_(cap) {}
  void push(double x) { std::lock_guard lk(mu_); data_.push_back(x); while(data_.size()>cap_) data_.pop_front(); }
  double sum() const { std::lock_guard lk(mu_); double s=0; for(double x:data_)s+=x; return s; }
  double mean() const { auto n=size(); return n?sum()/n:0; }
  std::size_t size() const { std::lock_guard lk(mu_); return data_.size(); }
  double at(std::size_t i) const { std::lock_guard lk(mu_); return i<data_.size()?data_[i]:0; }
private:
  std::size_t cap_; mutable std::mutex mu_; std::deque<double> data_;
};

class RingBuffers27 {
public:
  explicit RingBuffers27(std::size_t cap=256): cap_(cap) {}
  void push(double x) { std::lock_guard lk(mu_); data_.push_back(x); while(data_.size()>cap_) data_.pop_front(); }
  double sum() const { std::lock_guard lk(mu_); double s=0; for(double x:data_)s+=x; return s; }
  double mean() const { auto n=size(); return n?sum()/n:0; }
  std::size_t size() const { std::lock_guard lk(mu_); return data_.size(); }
  double at(std::size_t i) const { std::lock_guard lk(mu_); return i<data_.size()?data_[i]:0; }
private:
  std::size_t cap_; mutable std::mutex mu_; std::deque<double> data_;
};

class RingBuffers28 {
public:
  explicit RingBuffers28(std::size_t cap=256): cap_(cap) {}
  void push(double x) { std::lock_guard lk(mu_); data_.push_back(x); while(data_.size()>cap_) data_.pop_front(); }
  double sum() const { std::lock_guard lk(mu_); double s=0; for(double x:data_)s+=x; return s; }
  double mean() const { auto n=size(); return n?sum()/n:0; }
  std::size_t size() const { std::lock_guard lk(mu_); return data_.size(); }
  double at(std::size_t i) const { std::lock_guard lk(mu_); return i<data_.size()?data_[i]:0; }
private:
  std::size_t cap_; mutable std::mutex mu_; std::deque<double> data_;
};

class RingBuffers29 {
public:
  explicit RingBuffers29(std::size_t cap=256): cap_(cap) {}
  void push(double x) { std::lock_guard lk(mu_); data_.push_back(x); while(data_.size()>cap_) data_.pop_front(); }
  double sum() const { std::lock_guard lk(mu_); double s=0; for(double x:data_)s+=x; return s; }
  double mean() const { auto n=size(); return n?sum()/n:0; }
  std::size_t size() const { std::lock_guard lk(mu_); return data_.size(); }
  double at(std::size_t i) const { std::lock_guard lk(mu_); return i<data_.size()?data_[i]:0; }
private:
  std::size_t cap_; mutable std::mutex mu_; std::deque<double> data_;
};
}
