#pragma once
#include "ingestion/signing/hmac.hpp"
#include <string>
namespace crashcore {
inline std::string hashPassword(const std::string& password, const std::string& salt) {
  return hmacSha256Hex(salt, password);
}
inline bool verifyPassword(const std::string& password, const std::string& salt, const std::string& hash) {
  return hashPassword(password, salt) == hash;
}
} // namespace crashcore
