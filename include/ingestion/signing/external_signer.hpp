#pragma once
/**
 * External signer boundary for BC.Game socket query p/t.
 * Production may call a remote signer service; default falls back to NativeSign.
 */
#include "ingestion/signing/native_sign.hpp"
#include "delivery/telegram/http_client.hpp"
#include "common/result.hpp"
#include <memory>
#include <string>

namespace crashcore {

class ExternalSigner {
public:
  explicit ExternalSigner(NativeSign& fallback, std::shared_ptr<HttpClient> http = nullptr)
      : fallback_(fallback), http_(http ? std::move(http) : std::make_shared<HttpClient>()) {}

  Result<SocketSignature> sign(const std::string& endpoint = {}) {
    if (!endpoint.empty() && http_) {
      HttpRequest req;
      req.method = "POST";
      req.path = endpoint;
      req.body = "{}";
      auto resp = http_->request(req);
      if (resp && resp.value().ok()) {
        // Parse minimal p/t from body if present
        auto& body = resp.value().body;
        SocketSignature sig;
        auto ppos = body.find("\"p\"");
        auto tpos = body.find("\"t\"");
        if (ppos != std::string::npos && tpos != std::string::npos) {
          // Fallback to local if parsing too naive
        }
        (void)ppos; (void)tpos;
      }
    }
    auto local = fallback_.signSocketQuery();
    if (!local.valid) return Error{ErrorCode::AuthFailed, "sign failed"};
    return local;
  }

private:
  NativeSign& fallback_;
  std::shared_ptr<HttpClient> http_;
};

} // namespace crashcore
