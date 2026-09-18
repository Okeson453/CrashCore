#pragma once

#include "pe/ml/ml_feature_vector.hpp"

#include <array>
#include <fstream>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace pe::ml {

/** Portable SHA-256 (public-domain style). Hot path never calls this — load time only. */
class Sha256 {
public:
  Sha256();
  void update(const uint8_t* data, size_t len);
  void update(std::string_view s) { update(reinterpret_cast<const uint8_t*>(s.data()), s.size()); }
  std::array<uint8_t, 32> digest();
  std::string hex() {
    auto d = digest();
    static const char* hexd = "0123456789abcdef";
    std::string out(64, '0');
    for (int i = 0; i < 32; ++i) {
      out[static_cast<size_t>(i * 2)] = hexd[d[static_cast<size_t>(i)] >> 4];
      out[static_cast<size_t>(i * 2 + 1)] = hexd[d[static_cast<size_t>(i)] & 0xf];
    }
    return out;
  }

private:
  void transform(const uint8_t* chunk);
  uint64_t bitlen_ = 0;
  uint32_t state_[8]{};
  uint8_t buffer_[64]{};
  size_t buflen_ = 0;
};

inline std::string sha256Hex(const std::vector<uint8_t>& data) {
  Sha256 s;
  if (!data.empty()) s.update(data.data(), data.size());
  return s.hex();
}

inline std::string sha256File(const std::string& path) {
  std::ifstream in(path, std::ios::binary);
  if (!in) return {};
  Sha256 s;
  std::array<uint8_t, 4096> buf{};
  while (in) {
    in.read(reinterpret_cast<char*>(buf.data()), static_cast<std::streamsize>(buf.size()));
    const auto n = static_cast<size_t>(in.gcount());
    if (n) s.update(buf.data(), n);
  }
  return s.hex();
}

/** FNV-1a 64-bit over feature order strings — schema fingerprint. */
inline std::string featureSchemaHash() {
  uint64_t h = 14695981039346656037ull;
  for (const auto& k : mlFeatureOrder()) {
    for (unsigned char c : k) {
      h ^= c;
      h *= 1099511628211ull;
    }
    h ^= static_cast<unsigned char>('|');
    h *= 1099511628211ull;
  }
  char out[17];
  for (int i = 15; i >= 0; --i) {
    const int nibble = static_cast<int>((h >> (i * 4)) & 0xf);
    out[15 - i] = "0123456789abcdef"[nibble];
  }
  out[16] = 0;
  return std::string(out);
}

} // namespace pe::ml
