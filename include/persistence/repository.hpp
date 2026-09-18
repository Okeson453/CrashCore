#pragma once
#include "common/result.hpp"
#include "common/types.hpp"
#include <string>
#include <vector>
#include <optional>

namespace crashcore {

template <typename T, typename Id = std::string>
class Repository {
public:
  virtual ~Repository() = default;
  virtual Result<void> upsert(const T& entity) = 0;
  virtual Result<std::optional<T>> findById(const Id& id) = 0;
  virtual Result<std::vector<T>> findRecent(std::size_t limit) = 0;
};

} // namespace crashcore
