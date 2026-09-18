#pragma once
/**
 * ExternalSigner UDS client — CrashCore talks to a Node process that runs
 * wr_utils.wasm inside node:vm (matches TE sandboxing without embedding V8).
 *
 * Protocol (line-delimited JSON over Unix domain socket):
 *   request:  {"op":"sign","ua":"...","ts":123}\n
 *   response: {"ok":true,"p":"...","t":"..."}\n
 */
#include "ingestion/signing/signature.hpp"
#include "common/result.hpp"
#include "common/errors.hpp"
#include "timing/timestamp.hpp"
#include <atomic>
#include <cstring>
#include <mutex>
#include <string>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

namespace crashcore {

class ExternalSignerClient {
public:
  explicit ExternalSignerClient(std::string socketPath = "/tmp/crashcore-signer.sock")
      : path_(std::move(socketPath)) {}

  ~ExternalSignerClient() { disconnect(); }

  Result<void> connect() {
    std::lock_guard lk(mu_);
    if (fd_ >= 0) return Result<void>::success();
    fd_ = ::socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd_ < 0) return Error{ErrorCode::NetworkError, "socket() failed"};

    sockaddr_un addr{};
    addr.sun_family = AF_UNIX;
    std::strncpy(addr.sun_path, path_.c_str(), sizeof(addr.sun_path) - 1);

    if (::connect(fd_, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
      ::close(fd_);
      fd_ = -1;
      return Error{ErrorCode::ConnectionFailed, "connect to signer UDS failed: " + path_};
    }
    connected_.store(true);
    return Result<void>::success();
  }

  void disconnect() {
    std::lock_guard lk(mu_);
    if (fd_ >= 0) {
      ::close(fd_);
      fd_ = -1;
    }
    connected_.store(false);
  }

  bool connected() const noexcept { return connected_.load(); }

  Result<SocketSignature> sign(const std::string& userAgent) {
    std::lock_guard lk(mu_);
    if (fd_ < 0) {
      mu_.unlock();
      auto cr = connect();
      mu_.lock();
      if (!cr) return Error{cr.error().code, cr.error().message};
    }

    const auto ts = nowMs();
    std::string req = "{\"op\":\"sign\",\"ua\":\"";
    for (char c : userAgent) {
      if (c == '"' || c == '\\') req.push_back('\\');
      req.push_back(c);
    }
    req += "\",\"ts\":" + std::to_string(ts) + "}\n";

    const ssize_t nw = ::write(fd_, req.data(), req.size());
    if (nw < 0 || static_cast<std::size_t>(nw) != req.size()) {
      ++failures_;
      if (fd_ >= 0) { ::close(fd_); fd_ = -1; }
      connected_.store(false);
      return Error{ErrorCode::NetworkError, "signer write failed"};
    }

    char buf[2048];
    const ssize_t nr = ::read(fd_, buf, sizeof(buf) - 1);
    if (nr <= 0) {
      ++failures_;
      if (fd_ >= 0) { ::close(fd_); fd_ = -1; }
      connected_.store(false);
      return Error{ErrorCode::NetworkError, "signer read failed"};
    }
    buf[nr] = '\0';
    std::string resp(buf, static_cast<std::size_t>(nr));

    auto extract = [&](const char* key) -> std::string {
      std::string needle = std::string("\"") + key + "\":\"";
      auto pos = resp.find(needle);
      if (pos == std::string::npos) return {};
      pos += needle.size();
      auto end = resp.find('"', pos);
      if (end == std::string::npos) return {};
      return resp.substr(pos, end - pos);
    };

    if (resp.find("\"ok\":true") == std::string::npos &&
        resp.find("\"ok\": true") == std::string::npos) {
      ++failures_;
      return Error{ErrorCode::AuthFailed, "signer rejected: " + resp};
    }
    SocketSignature sig;
    sig.p = extract("p");
    sig.t = extract("t");
    sig.ua = userAgent;
    sig.at_ms = ts;
    sig.valid = !sig.p.empty() && !sig.t.empty();
    if (!sig.valid) {
      ++failures_;
      return Error{ErrorCode::AuthFailed, "signer response missing p/t"};
    }
    ++successes_;
    return sig;
  }

  std::uint64_t successCount() const noexcept { return successes_.load(); }
  std::uint64_t failureCount() const noexcept { return failures_.load(); }

private:
  std::string path_;
  int fd_ = -1;
  std::mutex mu_;
  std::atomic<bool> connected_{false};
  std::atomic<std::uint64_t> successes_{0};
  std::atomic<std::uint64_t> failures_{0};
};

} // namespace crashcore
