#include "ingestion/signing/native_sign.hpp"

namespace crashcore {

const char* kNativeSignModule = "crashcore.ingestion.signing.native_sign";

SocketSignature nativeSignQuery(NativeSign& sign) {
  return sign.signSocketQuery();
}

bool nativeSignReady(const NativeSign& sign) {
  return sign.isReady();
}

} // namespace crashcore
