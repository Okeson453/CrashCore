#pragma once

#include <cstddef>
#include <vector>

namespace pe::ml {

class MLNormalizer {
public:
  MLNormalizer() = default;
  MLNormalizer(std::vector<double> mean, std::vector<double> scale);

  bool empty() const noexcept { return mean_.empty(); }
  void transform(const double* input, double* output, std::size_t count) const noexcept;

private:
  std::vector<double> mean_;
  std::vector<double> scale_;
};

} // namespace pe::ml
