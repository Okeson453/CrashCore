#pragma once
#include "common/enums.hpp"
#include "common/types.hpp"
#include "timing/timestamp.hpp"
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace crashcore {

class LogFields {
public:
  LogFields& add(std::string key, std::string value) {
    fields_.emplace_back(std::move(key), std::move(value));
    return *this;
  }
  LogFields& add(std::string key, std::int64_t value) {
    fields_.emplace_back(std::move(key), std::to_string(value));
    return *this;
  }
  LogFields& add(std::string key, double value) {
    fields_.emplace_back(std::move(key), std::to_string(value));
    return *this;
  }
  LogFields& add(std::string key, bool value) {
    fields_.emplace_back(std::move(key), value ? "true" : "false");
    return *this;
  }
  std::string toString() const {
    std::ostringstream oss;
    bool first = true;
    for (const auto& [k, v] : fields_) {
      if (!first) oss << ' ';
      first = false;
      oss << k << '=' << v;
    }
    return oss.str();
  }
  std::string toJsonLike() const {
    std::ostringstream oss;
    oss << '{';
    bool first = true;
    for (const auto& [k, v] : fields_) {
      if (!first) oss << ',';
      first = false;
      oss << '"' << k << "\":\"" << v << '"';
    }
    oss << '}';
    return oss.str();
  }
private:
  std::vector<std::pair<std::string, std::string>> fields_;
};

} // namespace crashcore
