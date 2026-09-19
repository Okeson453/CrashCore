#include "security/secret_provider.hpp"

namespace crashcore {

const char* kSecretProviderModule = "crashcore.security.secret_provider";

std::string secretOrEmpty(const SecretProvider& sp, const std::string& key) {
  return sp.get(key).value_or("");
}

} // namespace crashcore
