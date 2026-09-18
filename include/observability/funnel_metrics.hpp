#pragma once
#include <atomic>
#include <string>
namespace crashcore {
class FunnelMetrics {
public:
  void noteFrame() { ++frames_; }
  void noteDecoded() { ++decoded_; }
  void noteRouted() { ++routed_; }
  void notePredicted() { ++predicted_; }
  void noteActionable() { ++actionable_; }
  void noteValidated() { ++validated_; }
  void notePublished() { ++published_; }
  void noteDelivered() { ++delivered_; }
  std::string report() const {
    return "frames="+std::to_string(frames_.load())
      +" decoded="+std::to_string(decoded_.load())
      +" routed="+std::to_string(routed_.load())
      +" predicted="+std::to_string(predicted_.load())
      +" actionable="+std::to_string(actionable_.load())
      +" validated="+std::to_string(validated_.load())
      +" published="+std::to_string(published_.load())
      +" delivered="+std::to_string(delivered_.load());
  }
  double dropRate() const {
    auto f=frames_.load(); if(!f) return 0; return 1.0 - double(decoded_.load())/double(f);
  }
private:
  std::atomic<std::uint64_t> frames_{0},decoded_{0},routed_{0},predicted_{0},actionable_{0},validated_{0},published_{0},delivered_{0};
};
} // namespace crashcore
