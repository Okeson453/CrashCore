#pragma once
#include <array>
#include <mutex>
#include <cmath>
namespace crashcore{

class FixedHist0 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    int b = int(std::clamp(x,0.0,19.99));
    if(b>=0&&b<20) counts_[b]++;
    ++n_;
  }
  double density(int b) const { std::lock_guard lk(mu_); return n_?double(counts_[b])/n_:0; }
  std::uint64_t total() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_; std::array<std::uint64_t,20> counts_{}; std::uint64_t n_{0};
};

class FixedHist1 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    int b = int(std::clamp(x,0.0,19.99));
    if(b>=0&&b<20) counts_[b]++;
    ++n_;
  }
  double density(int b) const { std::lock_guard lk(mu_); return n_?double(counts_[b])/n_:0; }
  std::uint64_t total() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_; std::array<std::uint64_t,20> counts_{}; std::uint64_t n_{0};
};

class FixedHist2 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    int b = int(std::clamp(x,0.0,19.99));
    if(b>=0&&b<20) counts_[b]++;
    ++n_;
  }
  double density(int b) const { std::lock_guard lk(mu_); return n_?double(counts_[b])/n_:0; }
  std::uint64_t total() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_; std::array<std::uint64_t,20> counts_{}; std::uint64_t n_{0};
};

class FixedHist3 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    int b = int(std::clamp(x,0.0,19.99));
    if(b>=0&&b<20) counts_[b]++;
    ++n_;
  }
  double density(int b) const { std::lock_guard lk(mu_); return n_?double(counts_[b])/n_:0; }
  std::uint64_t total() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_; std::array<std::uint64_t,20> counts_{}; std::uint64_t n_{0};
};

class FixedHist4 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    int b = int(std::clamp(x,0.0,19.99));
    if(b>=0&&b<20) counts_[b]++;
    ++n_;
  }
  double density(int b) const { std::lock_guard lk(mu_); return n_?double(counts_[b])/n_:0; }
  std::uint64_t total() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_; std::array<std::uint64_t,20> counts_{}; std::uint64_t n_{0};
};

class FixedHist5 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    int b = int(std::clamp(x,0.0,19.99));
    if(b>=0&&b<20) counts_[b]++;
    ++n_;
  }
  double density(int b) const { std::lock_guard lk(mu_); return n_?double(counts_[b])/n_:0; }
  std::uint64_t total() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_; std::array<std::uint64_t,20> counts_{}; std::uint64_t n_{0};
};

class FixedHist6 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    int b = int(std::clamp(x,0.0,19.99));
    if(b>=0&&b<20) counts_[b]++;
    ++n_;
  }
  double density(int b) const { std::lock_guard lk(mu_); return n_?double(counts_[b])/n_:0; }
  std::uint64_t total() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_; std::array<std::uint64_t,20> counts_{}; std::uint64_t n_{0};
};

class FixedHist7 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    int b = int(std::clamp(x,0.0,19.99));
    if(b>=0&&b<20) counts_[b]++;
    ++n_;
  }
  double density(int b) const { std::lock_guard lk(mu_); return n_?double(counts_[b])/n_:0; }
  std::uint64_t total() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_; std::array<std::uint64_t,20> counts_{}; std::uint64_t n_{0};
};

class FixedHist8 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    int b = int(std::clamp(x,0.0,19.99));
    if(b>=0&&b<20) counts_[b]++;
    ++n_;
  }
  double density(int b) const { std::lock_guard lk(mu_); return n_?double(counts_[b])/n_:0; }
  std::uint64_t total() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_; std::array<std::uint64_t,20> counts_{}; std::uint64_t n_{0};
};

class FixedHist9 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    int b = int(std::clamp(x,0.0,19.99));
    if(b>=0&&b<20) counts_[b]++;
    ++n_;
  }
  double density(int b) const { std::lock_guard lk(mu_); return n_?double(counts_[b])/n_:0; }
  std::uint64_t total() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_; std::array<std::uint64_t,20> counts_{}; std::uint64_t n_{0};
};

class FixedHist10 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    int b = int(std::clamp(x,0.0,19.99));
    if(b>=0&&b<20) counts_[b]++;
    ++n_;
  }
  double density(int b) const { std::lock_guard lk(mu_); return n_?double(counts_[b])/n_:0; }
  std::uint64_t total() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_; std::array<std::uint64_t,20> counts_{}; std::uint64_t n_{0};
};

class FixedHist11 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    int b = int(std::clamp(x,0.0,19.99));
    if(b>=0&&b<20) counts_[b]++;
    ++n_;
  }
  double density(int b) const { std::lock_guard lk(mu_); return n_?double(counts_[b])/n_:0; }
  std::uint64_t total() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_; std::array<std::uint64_t,20> counts_{}; std::uint64_t n_{0};
};

class FixedHist12 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    int b = int(std::clamp(x,0.0,19.99));
    if(b>=0&&b<20) counts_[b]++;
    ++n_;
  }
  double density(int b) const { std::lock_guard lk(mu_); return n_?double(counts_[b])/n_:0; }
  std::uint64_t total() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_; std::array<std::uint64_t,20> counts_{}; std::uint64_t n_{0};
};

class FixedHist13 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    int b = int(std::clamp(x,0.0,19.99));
    if(b>=0&&b<20) counts_[b]++;
    ++n_;
  }
  double density(int b) const { std::lock_guard lk(mu_); return n_?double(counts_[b])/n_:0; }
  std::uint64_t total() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_; std::array<std::uint64_t,20> counts_{}; std::uint64_t n_{0};
};

class FixedHist14 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    int b = int(std::clamp(x,0.0,19.99));
    if(b>=0&&b<20) counts_[b]++;
    ++n_;
  }
  double density(int b) const { std::lock_guard lk(mu_); return n_?double(counts_[b])/n_:0; }
  std::uint64_t total() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_; std::array<std::uint64_t,20> counts_{}; std::uint64_t n_{0};
};

class FixedHist15 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    int b = int(std::clamp(x,0.0,19.99));
    if(b>=0&&b<20) counts_[b]++;
    ++n_;
  }
  double density(int b) const { std::lock_guard lk(mu_); return n_?double(counts_[b])/n_:0; }
  std::uint64_t total() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_; std::array<std::uint64_t,20> counts_{}; std::uint64_t n_{0};
};

class FixedHist16 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    int b = int(std::clamp(x,0.0,19.99));
    if(b>=0&&b<20) counts_[b]++;
    ++n_;
  }
  double density(int b) const { std::lock_guard lk(mu_); return n_?double(counts_[b])/n_:0; }
  std::uint64_t total() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_; std::array<std::uint64_t,20> counts_{}; std::uint64_t n_{0};
};

class FixedHist17 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    int b = int(std::clamp(x,0.0,19.99));
    if(b>=0&&b<20) counts_[b]++;
    ++n_;
  }
  double density(int b) const { std::lock_guard lk(mu_); return n_?double(counts_[b])/n_:0; }
  std::uint64_t total() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_; std::array<std::uint64_t,20> counts_{}; std::uint64_t n_{0};
};

class FixedHist18 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    int b = int(std::clamp(x,0.0,19.99));
    if(b>=0&&b<20) counts_[b]++;
    ++n_;
  }
  double density(int b) const { std::lock_guard lk(mu_); return n_?double(counts_[b])/n_:0; }
  std::uint64_t total() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_; std::array<std::uint64_t,20> counts_{}; std::uint64_t n_{0};
};

class FixedHist19 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    int b = int(std::clamp(x,0.0,19.99));
    if(b>=0&&b<20) counts_[b]++;
    ++n_;
  }
  double density(int b) const { std::lock_guard lk(mu_); return n_?double(counts_[b])/n_:0; }
  std::uint64_t total() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_; std::array<std::uint64_t,20> counts_{}; std::uint64_t n_{0};
};

class FixedHist20 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    int b = int(std::clamp(x,0.0,19.99));
    if(b>=0&&b<20) counts_[b]++;
    ++n_;
  }
  double density(int b) const { std::lock_guard lk(mu_); return n_?double(counts_[b])/n_:0; }
  std::uint64_t total() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_; std::array<std::uint64_t,20> counts_{}; std::uint64_t n_{0};
};

class FixedHist21 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    int b = int(std::clamp(x,0.0,19.99));
    if(b>=0&&b<20) counts_[b]++;
    ++n_;
  }
  double density(int b) const { std::lock_guard lk(mu_); return n_?double(counts_[b])/n_:0; }
  std::uint64_t total() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_; std::array<std::uint64_t,20> counts_{}; std::uint64_t n_{0};
};

class FixedHist22 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    int b = int(std::clamp(x,0.0,19.99));
    if(b>=0&&b<20) counts_[b]++;
    ++n_;
  }
  double density(int b) const { std::lock_guard lk(mu_); return n_?double(counts_[b])/n_:0; }
  std::uint64_t total() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_; std::array<std::uint64_t,20> counts_{}; std::uint64_t n_{0};
};

class FixedHist23 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    int b = int(std::clamp(x,0.0,19.99));
    if(b>=0&&b<20) counts_[b]++;
    ++n_;
  }
  double density(int b) const { std::lock_guard lk(mu_); return n_?double(counts_[b])/n_:0; }
  std::uint64_t total() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_; std::array<std::uint64_t,20> counts_{}; std::uint64_t n_{0};
};

class FixedHist24 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    int b = int(std::clamp(x,0.0,19.99));
    if(b>=0&&b<20) counts_[b]++;
    ++n_;
  }
  double density(int b) const { std::lock_guard lk(mu_); return n_?double(counts_[b])/n_:0; }
  std::uint64_t total() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_; std::array<std::uint64_t,20> counts_{}; std::uint64_t n_{0};
};

class FixedHist25 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    int b = int(std::clamp(x,0.0,19.99));
    if(b>=0&&b<20) counts_[b]++;
    ++n_;
  }
  double density(int b) const { std::lock_guard lk(mu_); return n_?double(counts_[b])/n_:0; }
  std::uint64_t total() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_; std::array<std::uint64_t,20> counts_{}; std::uint64_t n_{0};
};

class FixedHist26 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    int b = int(std::clamp(x,0.0,19.99));
    if(b>=0&&b<20) counts_[b]++;
    ++n_;
  }
  double density(int b) const { std::lock_guard lk(mu_); return n_?double(counts_[b])/n_:0; }
  std::uint64_t total() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_; std::array<std::uint64_t,20> counts_{}; std::uint64_t n_{0};
};

class FixedHist27 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    int b = int(std::clamp(x,0.0,19.99));
    if(b>=0&&b<20) counts_[b]++;
    ++n_;
  }
  double density(int b) const { std::lock_guard lk(mu_); return n_?double(counts_[b])/n_:0; }
  std::uint64_t total() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_; std::array<std::uint64_t,20> counts_{}; std::uint64_t n_{0};
};

class FixedHist28 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    int b = int(std::clamp(x,0.0,19.99));
    if(b>=0&&b<20) counts_[b]++;
    ++n_;
  }
  double density(int b) const { std::lock_guard lk(mu_); return n_?double(counts_[b])/n_:0; }
  std::uint64_t total() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_; std::array<std::uint64_t,20> counts_{}; std::uint64_t n_{0};
};

class FixedHist29 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    int b = int(std::clamp(x,0.0,19.99));
    if(b>=0&&b<20) counts_[b]++;
    ++n_;
  }
  double density(int b) const { std::lock_guard lk(mu_); return n_?double(counts_[b])/n_:0; }
  std::uint64_t total() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_; std::array<std::uint64_t,20> counts_{}; std::uint64_t n_{0};
};

class FixedHist30 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    int b = int(std::clamp(x,0.0,19.99));
    if(b>=0&&b<20) counts_[b]++;
    ++n_;
  }
  double density(int b) const { std::lock_guard lk(mu_); return n_?double(counts_[b])/n_:0; }
  std::uint64_t total() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_; std::array<std::uint64_t,20> counts_{}; std::uint64_t n_{0};
};

class FixedHist31 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    int b = int(std::clamp(x,0.0,19.99));
    if(b>=0&&b<20) counts_[b]++;
    ++n_;
  }
  double density(int b) const { std::lock_guard lk(mu_); return n_?double(counts_[b])/n_:0; }
  std::uint64_t total() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_; std::array<std::uint64_t,20> counts_{}; std::uint64_t n_{0};
};

class FixedHist32 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    int b = int(std::clamp(x,0.0,19.99));
    if(b>=0&&b<20) counts_[b]++;
    ++n_;
  }
  double density(int b) const { std::lock_guard lk(mu_); return n_?double(counts_[b])/n_:0; }
  std::uint64_t total() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_; std::array<std::uint64_t,20> counts_{}; std::uint64_t n_{0};
};

class FixedHist33 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    int b = int(std::clamp(x,0.0,19.99));
    if(b>=0&&b<20) counts_[b]++;
    ++n_;
  }
  double density(int b) const { std::lock_guard lk(mu_); return n_?double(counts_[b])/n_:0; }
  std::uint64_t total() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_; std::array<std::uint64_t,20> counts_{}; std::uint64_t n_{0};
};

class FixedHist34 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    int b = int(std::clamp(x,0.0,19.99));
    if(b>=0&&b<20) counts_[b]++;
    ++n_;
  }
  double density(int b) const { std::lock_guard lk(mu_); return n_?double(counts_[b])/n_:0; }
  std::uint64_t total() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_; std::array<std::uint64_t,20> counts_{}; std::uint64_t n_{0};
};

class FixedHist35 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    int b = int(std::clamp(x,0.0,19.99));
    if(b>=0&&b<20) counts_[b]++;
    ++n_;
  }
  double density(int b) const { std::lock_guard lk(mu_); return n_?double(counts_[b])/n_:0; }
  std::uint64_t total() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_; std::array<std::uint64_t,20> counts_{}; std::uint64_t n_{0};
};

class FixedHist36 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    int b = int(std::clamp(x,0.0,19.99));
    if(b>=0&&b<20) counts_[b]++;
    ++n_;
  }
  double density(int b) const { std::lock_guard lk(mu_); return n_?double(counts_[b])/n_:0; }
  std::uint64_t total() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_; std::array<std::uint64_t,20> counts_{}; std::uint64_t n_{0};
};

class FixedHist37 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    int b = int(std::clamp(x,0.0,19.99));
    if(b>=0&&b<20) counts_[b]++;
    ++n_;
  }
  double density(int b) const { std::lock_guard lk(mu_); return n_?double(counts_[b])/n_:0; }
  std::uint64_t total() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_; std::array<std::uint64_t,20> counts_{}; std::uint64_t n_{0};
};

class FixedHist38 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    int b = int(std::clamp(x,0.0,19.99));
    if(b>=0&&b<20) counts_[b]++;
    ++n_;
  }
  double density(int b) const { std::lock_guard lk(mu_); return n_?double(counts_[b])/n_:0; }
  std::uint64_t total() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_; std::array<std::uint64_t,20> counts_{}; std::uint64_t n_{0};
};

class FixedHist39 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    int b = int(std::clamp(x,0.0,19.99));
    if(b>=0&&b<20) counts_[b]++;
    ++n_;
  }
  double density(int b) const { std::lock_guard lk(mu_); return n_?double(counts_[b])/n_:0; }
  std::uint64_t total() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_; std::array<std::uint64_t,20> counts_{}; std::uint64_t n_{0};
};

class FixedHist40 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    int b = int(std::clamp(x,0.0,19.99));
    if(b>=0&&b<20) counts_[b]++;
    ++n_;
  }
  double density(int b) const { std::lock_guard lk(mu_); return n_?double(counts_[b])/n_:0; }
  std::uint64_t total() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_; std::array<std::uint64_t,20> counts_{}; std::uint64_t n_{0};
};

class FixedHist41 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    int b = int(std::clamp(x,0.0,19.99));
    if(b>=0&&b<20) counts_[b]++;
    ++n_;
  }
  double density(int b) const { std::lock_guard lk(mu_); return n_?double(counts_[b])/n_:0; }
  std::uint64_t total() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_; std::array<std::uint64_t,20> counts_{}; std::uint64_t n_{0};
};

class FixedHist42 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    int b = int(std::clamp(x,0.0,19.99));
    if(b>=0&&b<20) counts_[b]++;
    ++n_;
  }
  double density(int b) const { std::lock_guard lk(mu_); return n_?double(counts_[b])/n_:0; }
  std::uint64_t total() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_; std::array<std::uint64_t,20> counts_{}; std::uint64_t n_{0};
};

class FixedHist43 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    int b = int(std::clamp(x,0.0,19.99));
    if(b>=0&&b<20) counts_[b]++;
    ++n_;
  }
  double density(int b) const { std::lock_guard lk(mu_); return n_?double(counts_[b])/n_:0; }
  std::uint64_t total() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_; std::array<std::uint64_t,20> counts_{}; std::uint64_t n_{0};
};

class FixedHist44 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    int b = int(std::clamp(x,0.0,19.99));
    if(b>=0&&b<20) counts_[b]++;
    ++n_;
  }
  double density(int b) const { std::lock_guard lk(mu_); return n_?double(counts_[b])/n_:0; }
  std::uint64_t total() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_; std::array<std::uint64_t,20> counts_{}; std::uint64_t n_{0};
};

class FixedHist45 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    int b = int(std::clamp(x,0.0,19.99));
    if(b>=0&&b<20) counts_[b]++;
    ++n_;
  }
  double density(int b) const { std::lock_guard lk(mu_); return n_?double(counts_[b])/n_:0; }
  std::uint64_t total() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_; std::array<std::uint64_t,20> counts_{}; std::uint64_t n_{0};
};

class FixedHist46 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    int b = int(std::clamp(x,0.0,19.99));
    if(b>=0&&b<20) counts_[b]++;
    ++n_;
  }
  double density(int b) const { std::lock_guard lk(mu_); return n_?double(counts_[b])/n_:0; }
  std::uint64_t total() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_; std::array<std::uint64_t,20> counts_{}; std::uint64_t n_{0};
};

class FixedHist47 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    int b = int(std::clamp(x,0.0,19.99));
    if(b>=0&&b<20) counts_[b]++;
    ++n_;
  }
  double density(int b) const { std::lock_guard lk(mu_); return n_?double(counts_[b])/n_:0; }
  std::uint64_t total() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_; std::array<std::uint64_t,20> counts_{}; std::uint64_t n_{0};
};

class FixedHist48 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    int b = int(std::clamp(x,0.0,19.99));
    if(b>=0&&b<20) counts_[b]++;
    ++n_;
  }
  double density(int b) const { std::lock_guard lk(mu_); return n_?double(counts_[b])/n_:0; }
  std::uint64_t total() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_; std::array<std::uint64_t,20> counts_{}; std::uint64_t n_{0};
};

class FixedHist49 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    int b = int(std::clamp(x,0.0,19.99));
    if(b>=0&&b<20) counts_[b]++;
    ++n_;
  }
  double density(int b) const { std::lock_guard lk(mu_); return n_?double(counts_[b])/n_:0; }
  std::uint64_t total() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_; std::array<std::uint64_t,20> counts_{}; std::uint64_t n_{0};
};

class FixedHist50 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    int b = int(std::clamp(x,0.0,19.99));
    if(b>=0&&b<20) counts_[b]++;
    ++n_;
  }
  double density(int b) const { std::lock_guard lk(mu_); return n_?double(counts_[b])/n_:0; }
  std::uint64_t total() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_; std::array<std::uint64_t,20> counts_{}; std::uint64_t n_{0};
};

class FixedHist51 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    int b = int(std::clamp(x,0.0,19.99));
    if(b>=0&&b<20) counts_[b]++;
    ++n_;
  }
  double density(int b) const { std::lock_guard lk(mu_); return n_?double(counts_[b])/n_:0; }
  std::uint64_t total() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_; std::array<std::uint64_t,20> counts_{}; std::uint64_t n_{0};
};

class FixedHist52 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    int b = int(std::clamp(x,0.0,19.99));
    if(b>=0&&b<20) counts_[b]++;
    ++n_;
  }
  double density(int b) const { std::lock_guard lk(mu_); return n_?double(counts_[b])/n_:0; }
  std::uint64_t total() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_; std::array<std::uint64_t,20> counts_{}; std::uint64_t n_{0};
};

class FixedHist53 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    int b = int(std::clamp(x,0.0,19.99));
    if(b>=0&&b<20) counts_[b]++;
    ++n_;
  }
  double density(int b) const { std::lock_guard lk(mu_); return n_?double(counts_[b])/n_:0; }
  std::uint64_t total() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_; std::array<std::uint64_t,20> counts_{}; std::uint64_t n_{0};
};

class FixedHist54 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    int b = int(std::clamp(x,0.0,19.99));
    if(b>=0&&b<20) counts_[b]++;
    ++n_;
  }
  double density(int b) const { std::lock_guard lk(mu_); return n_?double(counts_[b])/n_:0; }
  std::uint64_t total() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_; std::array<std::uint64_t,20> counts_{}; std::uint64_t n_{0};
};

class FixedHist55 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    int b = int(std::clamp(x,0.0,19.99));
    if(b>=0&&b<20) counts_[b]++;
    ++n_;
  }
  double density(int b) const { std::lock_guard lk(mu_); return n_?double(counts_[b])/n_:0; }
  std::uint64_t total() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_; std::array<std::uint64_t,20> counts_{}; std::uint64_t n_{0};
};

class FixedHist56 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    int b = int(std::clamp(x,0.0,19.99));
    if(b>=0&&b<20) counts_[b]++;
    ++n_;
  }
  double density(int b) const { std::lock_guard lk(mu_); return n_?double(counts_[b])/n_:0; }
  std::uint64_t total() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_; std::array<std::uint64_t,20> counts_{}; std::uint64_t n_{0};
};

class FixedHist57 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    int b = int(std::clamp(x,0.0,19.99));
    if(b>=0&&b<20) counts_[b]++;
    ++n_;
  }
  double density(int b) const { std::lock_guard lk(mu_); return n_?double(counts_[b])/n_:0; }
  std::uint64_t total() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_; std::array<std::uint64_t,20> counts_{}; std::uint64_t n_{0};
};

class FixedHist58 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    int b = int(std::clamp(x,0.0,19.99));
    if(b>=0&&b<20) counts_[b]++;
    ++n_;
  }
  double density(int b) const { std::lock_guard lk(mu_); return n_?double(counts_[b])/n_:0; }
  std::uint64_t total() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_; std::array<std::uint64_t,20> counts_{}; std::uint64_t n_{0};
};

class FixedHist59 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    int b = int(std::clamp(x,0.0,19.99));
    if(b>=0&&b<20) counts_[b]++;
    ++n_;
  }
  double density(int b) const { std::lock_guard lk(mu_); return n_?double(counts_[b])/n_:0; }
  std::uint64_t total() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_; std::array<std::uint64_t,20> counts_{}; std::uint64_t n_{0};
};

class FixedHist60 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    int b = int(std::clamp(x,0.0,19.99));
    if(b>=0&&b<20) counts_[b]++;
    ++n_;
  }
  double density(int b) const { std::lock_guard lk(mu_); return n_?double(counts_[b])/n_:0; }
  std::uint64_t total() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_; std::array<std::uint64_t,20> counts_{}; std::uint64_t n_{0};
};

class FixedHist61 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    int b = int(std::clamp(x,0.0,19.99));
    if(b>=0&&b<20) counts_[b]++;
    ++n_;
  }
  double density(int b) const { std::lock_guard lk(mu_); return n_?double(counts_[b])/n_:0; }
  std::uint64_t total() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_; std::array<std::uint64_t,20> counts_{}; std::uint64_t n_{0};
};

class FixedHist62 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    int b = int(std::clamp(x,0.0,19.99));
    if(b>=0&&b<20) counts_[b]++;
    ++n_;
  }
  double density(int b) const { std::lock_guard lk(mu_); return n_?double(counts_[b])/n_:0; }
  std::uint64_t total() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_; std::array<std::uint64_t,20> counts_{}; std::uint64_t n_{0};
};

class FixedHist63 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    int b = int(std::clamp(x,0.0,19.99));
    if(b>=0&&b<20) counts_[b]++;
    ++n_;
  }
  double density(int b) const { std::lock_guard lk(mu_); return n_?double(counts_[b])/n_:0; }
  std::uint64_t total() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_; std::array<std::uint64_t,20> counts_{}; std::uint64_t n_{0};
};

class FixedHist64 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    int b = int(std::clamp(x,0.0,19.99));
    if(b>=0&&b<20) counts_[b]++;
    ++n_;
  }
  double density(int b) const { std::lock_guard lk(mu_); return n_?double(counts_[b])/n_:0; }
  std::uint64_t total() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_; std::array<std::uint64_t,20> counts_{}; std::uint64_t n_{0};
};

class FixedHist65 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    int b = int(std::clamp(x,0.0,19.99));
    if(b>=0&&b<20) counts_[b]++;
    ++n_;
  }
  double density(int b) const { std::lock_guard lk(mu_); return n_?double(counts_[b])/n_:0; }
  std::uint64_t total() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_; std::array<std::uint64_t,20> counts_{}; std::uint64_t n_{0};
};

class FixedHist66 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    int b = int(std::clamp(x,0.0,19.99));
    if(b>=0&&b<20) counts_[b]++;
    ++n_;
  }
  double density(int b) const { std::lock_guard lk(mu_); return n_?double(counts_[b])/n_:0; }
  std::uint64_t total() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_; std::array<std::uint64_t,20> counts_{}; std::uint64_t n_{0};
};

class FixedHist67 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    int b = int(std::clamp(x,0.0,19.99));
    if(b>=0&&b<20) counts_[b]++;
    ++n_;
  }
  double density(int b) const { std::lock_guard lk(mu_); return n_?double(counts_[b])/n_:0; }
  std::uint64_t total() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_; std::array<std::uint64_t,20> counts_{}; std::uint64_t n_{0};
};

class FixedHist68 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    int b = int(std::clamp(x,0.0,19.99));
    if(b>=0&&b<20) counts_[b]++;
    ++n_;
  }
  double density(int b) const { std::lock_guard lk(mu_); return n_?double(counts_[b])/n_:0; }
  std::uint64_t total() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_; std::array<std::uint64_t,20> counts_{}; std::uint64_t n_{0};
};

class FixedHist69 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    int b = int(std::clamp(x,0.0,19.99));
    if(b>=0&&b<20) counts_[b]++;
    ++n_;
  }
  double density(int b) const { std::lock_guard lk(mu_); return n_?double(counts_[b])/n_:0; }
  std::uint64_t total() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_; std::array<std::uint64_t,20> counts_{}; std::uint64_t n_{0};
};

class FixedHist70 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    int b = int(std::clamp(x,0.0,19.99));
    if(b>=0&&b<20) counts_[b]++;
    ++n_;
  }
  double density(int b) const { std::lock_guard lk(mu_); return n_?double(counts_[b])/n_:0; }
  std::uint64_t total() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_; std::array<std::uint64_t,20> counts_{}; std::uint64_t n_{0};
};

class FixedHist71 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    int b = int(std::clamp(x,0.0,19.99));
    if(b>=0&&b<20) counts_[b]++;
    ++n_;
  }
  double density(int b) const { std::lock_guard lk(mu_); return n_?double(counts_[b])/n_:0; }
  std::uint64_t total() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_; std::array<std::uint64_t,20> counts_{}; std::uint64_t n_{0};
};

class FixedHist72 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    int b = int(std::clamp(x,0.0,19.99));
    if(b>=0&&b<20) counts_[b]++;
    ++n_;
  }
  double density(int b) const { std::lock_guard lk(mu_); return n_?double(counts_[b])/n_:0; }
  std::uint64_t total() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_; std::array<std::uint64_t,20> counts_{}; std::uint64_t n_{0};
};

class FixedHist73 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    int b = int(std::clamp(x,0.0,19.99));
    if(b>=0&&b<20) counts_[b]++;
    ++n_;
  }
  double density(int b) const { std::lock_guard lk(mu_); return n_?double(counts_[b])/n_:0; }
  std::uint64_t total() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_; std::array<std::uint64_t,20> counts_{}; std::uint64_t n_{0};
};

class FixedHist74 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    int b = int(std::clamp(x,0.0,19.99));
    if(b>=0&&b<20) counts_[b]++;
    ++n_;
  }
  double density(int b) const { std::lock_guard lk(mu_); return n_?double(counts_[b])/n_:0; }
  std::uint64_t total() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_; std::array<std::uint64_t,20> counts_{}; std::uint64_t n_{0};
};

class FixedHist75 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    int b = int(std::clamp(x,0.0,19.99));
    if(b>=0&&b<20) counts_[b]++;
    ++n_;
  }
  double density(int b) const { std::lock_guard lk(mu_); return n_?double(counts_[b])/n_:0; }
  std::uint64_t total() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_; std::array<std::uint64_t,20> counts_{}; std::uint64_t n_{0};
};

class FixedHist76 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    int b = int(std::clamp(x,0.0,19.99));
    if(b>=0&&b<20) counts_[b]++;
    ++n_;
  }
  double density(int b) const { std::lock_guard lk(mu_); return n_?double(counts_[b])/n_:0; }
  std::uint64_t total() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_; std::array<std::uint64_t,20> counts_{}; std::uint64_t n_{0};
};

class FixedHist77 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    int b = int(std::clamp(x,0.0,19.99));
    if(b>=0&&b<20) counts_[b]++;
    ++n_;
  }
  double density(int b) const { std::lock_guard lk(mu_); return n_?double(counts_[b])/n_:0; }
  std::uint64_t total() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_; std::array<std::uint64_t,20> counts_{}; std::uint64_t n_{0};
};

class FixedHist78 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    int b = int(std::clamp(x,0.0,19.99));
    if(b>=0&&b<20) counts_[b]++;
    ++n_;
  }
  double density(int b) const { std::lock_guard lk(mu_); return n_?double(counts_[b])/n_:0; }
  std::uint64_t total() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_; std::array<std::uint64_t,20> counts_{}; std::uint64_t n_{0};
};

class FixedHist79 {
public:
  void add(double x) {
    std::lock_guard lk(mu_);
    int b = int(std::clamp(x,0.0,19.99));
    if(b>=0&&b<20) counts_[b]++;
    ++n_;
  }
  double density(int b) const { std::lock_guard lk(mu_); return n_?double(counts_[b])/n_:0; }
  std::uint64_t total() const { std::lock_guard lk(mu_); return n_; }
private:
  mutable std::mutex mu_; std::array<std::uint64_t,20> counts_{}; std::uint64_t n_{0};
};
}
