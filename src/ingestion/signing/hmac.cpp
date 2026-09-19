#include "ingestion/signing/hmac.hpp"
#include "ingestion/signing/crypto.hpp"
#include <string>

namespace crashcore {

const char* kHmacModule = "crashcore.ingestion.signing.hmac";

std::string hmacSha256HexPublic(const std::string& key, const std::string& data) {
  return hmacSha256Hex(key, data);
}

} // namespace crashcore
