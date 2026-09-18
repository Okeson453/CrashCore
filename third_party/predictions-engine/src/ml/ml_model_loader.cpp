#include "pe/ml/ml_model_loader.hpp"
#include "pe/ml/ml_feature_vector.hpp"
#include "pe/ml/ml_logistic_model.hpp"
#include "pe/ml/ml_sha256.hpp"

#include <cmath>
#include <fstream>
#include <sstream>
#include <vector>

namespace pe::ml {
namespace {

template <typename T>
bool readPod(std::ifstream& in, T& v) {
  in.read(reinterpret_cast<char*>(&v), sizeof(T));
  return static_cast<bool>(in);
}

std::string readChecksumFile(const std::string& path) {
  std::ifstream in(path);
  if (!in) return {};
  std::string line;
  std::getline(in, line);
  // accept "hash" or "hash  filename"
  const auto sp = line.find_first_of(" \t");
  if (sp != std::string::npos) line = line.substr(0, sp);
  // trim
  while (!line.empty() && (line.back() == '\r' || line.back() == '\n')) line.pop_back();
  return line;
}

} // namespace

std::optional<LoadedModel> loadModelFromBinary(const std::string& modelPath,
                                               const std::string& checksumPath,
                                               const std::string& schemaPath,
                                               const std::string& metadataPath) {
  // 1) Checksum (plan §10)
  if (!checksumPath.empty()) {
    const std::string expected = readChecksumFile(checksumPath);
    const std::string actual = sha256File(modelPath);
    if (expected.empty() || actual.empty() || expected != actual)
      return std::nullopt;
  }

  // 2) Schema path: one feature name per line; must match ML_FEATURE_ORDER exactly
  if (!schemaPath.empty()) {
    std::ifstream sin(schemaPath);
    if (!sin) return std::nullopt;
    std::vector<std::string> names;
    std::string line;
    while (std::getline(sin, line)) {
      while (!line.empty() && (line.back()=='\r' || line.back()==' ' || line.back()=='\t')) line.pop_back();
      if (line.empty() || line[0]=='#') continue;
      names.push_back(line);
    }
    const auto& order = mlFeatureOrder();
    if (names.size() != order.size()) return std::nullopt;
    for (std::size_t i = 0; i < order.size(); ++i)
      if (names[i] != order[i]) return std::nullopt;
  }

  // 3) Metadata path (optional key=value lines); require feature_schema_hash match if present
  std::string meta_schema_hash;
  if (!metadataPath.empty()) {
    std::ifstream min(metadataPath);
    if (!min) return std::nullopt;
    std::string line;
    while (std::getline(min, line)) {
      const auto eq = line.find('=');
      if (eq == std::string::npos) continue;
      const std::string key = line.substr(0, eq);
      std::string val = line.substr(eq + 1);
      while (!val.empty() && (val.back()=='\r' || val.back()=='\n')) val.pop_back();
      if (key == "feature_schema_hash") meta_schema_hash = val;
    }
    if (!meta_schema_hash.empty() && meta_schema_hash != featureSchemaHash())
      return std::nullopt;
  }

  std::ifstream in(modelPath, std::ios::binary);
  if (!in) return std::nullopt;

  std::uint32_t magic = 0, version = 0, feature_count = 0;
  if (!readPod(in, magic) || magic != 0x50454D4Cu) return std::nullopt;
  if (!readPod(in, version) || version != 1) return std::nullopt;
  if (!readPod(in, feature_count) || feature_count == 0 || feature_count > MAX_ML_FEATURES)
    return std::nullopt;

  // Fail closed on schema size mismatch
  const auto expected_n = mlFeatureOrder().size();
  if (feature_count != expected_n) return std::nullopt;

  double bias = 0.0;
  if (!readPod(in, bias)) return std::nullopt;
  std::vector<double> weights(feature_count);
  for (std::size_t i = 0; i < feature_count; ++i)
    if (!readPod(in, weights[i])) return std::nullopt;

  LoadedModel loaded;
  MLModelMetadata meta;
  meta.model_id = "logistic";
  meta.version = "1";
  meta.feature_count = feature_count;
  meta.feature_schema_hash = featureSchemaHash();
  loaded.model = std::make_unique<MLLogisticModel>(meta, std::move(weights), bias);
  loaded.metadata = loaded.model->metadata();

  std::uint8_t has_norm = 0;
  if (!readPod(in, has_norm)) return std::nullopt;
  if (has_norm) {
    std::vector<double> mean(feature_count), scale(feature_count);
    for (std::size_t i = 0; i < feature_count; ++i) if (!readPod(in, mean[i])) return std::nullopt;
    for (std::size_t i = 0; i < feature_count; ++i) if (!readPod(in, scale[i])) return std::nullopt;
    loaded.normalizer = MLNormalizer(std::move(mean), std::move(scale));
    loaded.has_normalizer = true;
  }

  std::uint8_t cal_method = 0;
  if (!readPod(in, cal_method)) return std::nullopt;
  if (cal_method == 1) {
    double a = 1.0, b = 0.0;
    if (!readPod(in, a) || !readPod(in, b)) return std::nullopt;
    loaded.calibrator = MLCalibrator::platt(a, b);
    loaded.has_calibrator = true;
  } else if (cal_method == 2) {
    std::uint32_t n = 0;
    if (!readPod(in, n) || n == 0 || n > 1024) return std::nullopt;
    std::vector<double> x(n), y(n);
    for (std::uint32_t i = 0; i < n; ++i) if (!readPod(in, x[i])) return std::nullopt;
    for (std::uint32_t i = 0; i < n; ++i) if (!readPod(in, y[i])) return std::nullopt;
    loaded.calibrator = MLCalibrator::isotonic(std::move(x), std::move(y));
    loaded.has_calibrator = true;
  }

  std::vector<double> zeros(feature_count, 0.0);
  MLFeatureVector fv{zeros.data(), feature_count};
  if (!loaded.model->predict(fv).valid) return std::nullopt;

  return loaded;
}

LoadedModel makeDefaultLogisticModel(std::size_t feature_count) {
  if (feature_count == 0) feature_count = mlFeatureOrder().size();
  const double p0 = 0.65;
  const double bias = std::log(p0 / (1.0 - p0));
  std::vector<double> weights(feature_count, 0.0);
  const auto& order = mlFeatureOrder();
  for (std::size_t i = 0; i < order.size() && i < feature_count; ++i) {
    if (order[i] == "ewma_hit_13") weights[i] = 1.2;
    else if (order[i] == "markov_p_up") weights[i] = 0.4;
    else if (order[i] == "short_hit_13") weights[i] = 0.5;
    else if (order[i] == "entropy_binary_13") weights[i] = -0.2;
  }
  MLModelMetadata meta;
  meta.model_id = "logistic-default";
  meta.version = "1.0.0";
  meta.feature_count = feature_count;
  meta.feature_schema_hash = featureSchemaHash();
  LoadedModel loaded;
  loaded.model = std::make_unique<MLLogisticModel>(meta, std::move(weights), bias);
  loaded.metadata = loaded.model->metadata();
  loaded.calibrator = MLCalibrator::platt(1.0, 0.0);
  loaded.has_calibrator = true;
  return loaded;
}

} // namespace pe::ml
