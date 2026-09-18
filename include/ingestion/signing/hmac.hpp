#pragma once
#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <string>
#include <string_view>
#include <vector>
#include <cstdint>
#include <iomanip>
#include <sstream>

namespace crashcore {

inline std::vector<std::uint8_t> hmacSha256Raw(std::string_view key, std::string_view data) {
  std::vector<std::uint8_t> out(EVP_MAX_MD_SIZE);
  unsigned int len = 0;
  HMAC(EVP_sha256(),
       key.data(), static_cast<int>(key.size()),
       reinterpret_cast<const unsigned char*>(data.data()), data.size(),
       out.data(), &len);
  out.resize(len);
  return out;
}

inline std::string toHex(const std::vector<std::uint8_t>& bytes) {
  std::ostringstream oss;
  oss << std::hex << std::setfill('0');
  for (auto b : bytes) oss << std::setw(2) << static_cast<int>(b);
  return oss.str();
}

inline std::string hmacSha256Hex(std::string_view key, std::string_view data) {
  return toHex(hmacSha256Raw(key, data));
}

inline std::string sha256Hex(std::string_view data) {
  unsigned char hash[EVP_MAX_MD_SIZE];
  unsigned int len = 0;
  EVP_MD_CTX* ctx = EVP_MD_CTX_new();
  EVP_DigestInit_ex(ctx, EVP_sha256(), nullptr);
  EVP_DigestUpdate(ctx, data.data(), data.size());
  EVP_DigestFinal_ex(ctx, hash, &len);
  EVP_MD_CTX_free(ctx);
  return toHex(std::vector<std::uint8_t>(hash, hash + len));
}

} // namespace crashcore
