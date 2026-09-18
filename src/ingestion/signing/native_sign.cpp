#include "ingestion/signing/native_sign.hpp"
namespace crashcore {
// setExternalSigner(ExternalSignerClient*) for production wr_utils UDS signing.
// HMAC fallback is for offline tests only — bc.game rejects non-WASM signatures.
} // namespace crashcore
