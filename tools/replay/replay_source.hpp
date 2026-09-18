#pragma once
/**
 * Replay source — reads recorded binary frames and injects into CrashFeed
 * at original or accelerated timing. Used for offline protocol/validation tests.
 */
#include "common/types.hpp"
#include "timing/timestamp.hpp"
#include <cstdint>
#include <fstream>
#include <string>
#include <vector>
#include <optional>
#include <cstring>

namespace crashcore {

struct RecordedFrame {
  TimestampMs timestampMs = 0;
  std::vector<std::uint8_t> data;
};

class ReplaySource {
public:
  explicit ReplaySource(std::string path) : path_(std::move(path)) {}

  bool load() {
    std::ifstream in(path_, std::ios::binary);
    if (!in) return false;
    frames_.clear();
    // Simple format: [u64 ts_ms][u32 len][bytes...] repeated
    while (in) {
      std::uint64_t ts = 0;
      std::uint32_t len = 0;
      in.read(reinterpret_cast<char*>(&ts), sizeof(ts));
      if (!in) break;
      in.read(reinterpret_cast<char*>(&len), sizeof(len));
      if (!in || len > 16 * 1024 * 1024) break;
      RecordedFrame f;
      f.timestampMs = static_cast<TimestampMs>(ts);
      f.data.resize(len);
      in.read(reinterpret_cast<char*>(f.data.data()), len);
      if (!in) break;
      frames_.push_back(std::move(f));
    }
    cursor_ = 0;
    return !frames_.empty();
  }

  /** Synthetic loader for tests without a file. */
  void loadSynthetic(std::vector<RecordedFrame> frames) {
    frames_ = std::move(frames);
    cursor_ = 0;
  }

  std::optional<RecordedFrame> next() {
    if (cursor_ >= frames_.size()) return std::nullopt;
    return frames_[cursor_++];
  }

  void reset() { cursor_ = 0; }
  std::size_t size() const noexcept { return frames_.size(); }
  std::size_t remaining() const noexcept {
    return cursor_ < frames_.size() ? frames_.size() - cursor_ : 0;
  }

  static void writeFrame(std::ostream& out, TimestampMs ts, const std::uint8_t* data, std::size_t len) {
    std::uint64_t t = static_cast<std::uint64_t>(ts);
    std::uint32_t n = static_cast<std::uint32_t>(len);
    out.write(reinterpret_cast<const char*>(&t), sizeof(t));
    out.write(reinterpret_cast<const char*>(&n), sizeof(n));
    out.write(reinterpret_cast<const char*>(data), static_cast<std::streamsize>(len));
  }

private:
  std::string path_;
  std::vector<RecordedFrame> frames_;
  std::size_t cursor_ = 0;
};

} // namespace crashcore
