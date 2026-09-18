#include "pe/ml/ml_model_loader.hpp"
#include "pe/ml/ml_sha256.hpp"
#include "pe/ml/ml_feature_vector.hpp"
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>

static int failures = 0;
#define CHECK(c) do { if (!(c)) { std::cerr << "FAIL " << #c << " @" << __LINE__ << "\n"; ++failures; } } while (0)

static void writePemel(const std::string& path, uint32_t nfeat, double bias) {
  std::ofstream out(path, std::ios::binary);
  uint32_t magic = 0x50454D4C, ver = 1;
  out.write(reinterpret_cast<char*>(&magic), 4);
  out.write(reinterpret_cast<char*>(&ver), 4);
  out.write(reinterpret_cast<char*>(&nfeat), 4);
  out.write(reinterpret_cast<char*>(&bias), 8);
  double w = 0.0;
  for (uint32_t i = 0; i < nfeat; ++i) out.write(reinterpret_cast<char*>(&w), 8);
  uint8_t has_norm = 0, cal = 0;
  out.write(reinterpret_cast<char*>(&has_norm), 1);
  out.write(reinterpret_cast<char*>(&cal), 1);
}

int main() {
  const auto n = static_cast<uint32_t>(pe::ml::mlFeatureOrder().size());
  writePemel("/tmp/pe_ok.bin", n, 0.6);
  auto ok = pe::ml::loadModelFromBinary("/tmp/pe_ok.bin");
  CHECK(ok.has_value());
  CHECK(ok->model->featureCount() == n);

  writePemel("/tmp/pe_bad.bin", 3, 0.6);
  CHECK(!pe::ml::loadModelFromBinary("/tmp/pe_bad.bin").has_value());

  const std::string actual = pe::ml::sha256File("/tmp/pe_ok.bin");
  {
    std::ofstream("/tmp/pe_ok.sha") << "deadbeef\n";
  }
  CHECK(!pe::ml::loadModelFromBinary("/tmp/pe_ok.bin", "/tmp/pe_ok.sha").has_value());
  {
    std::ofstream("/tmp/pe_ok.sha") << actual << "\n";
  }
  CHECK(pe::ml::loadModelFromBinary("/tmp/pe_ok.bin", "/tmp/pe_ok.sha").has_value());

  // schema path must match ML_FEATURE_ORDER
  {
    std::ofstream sch("/tmp/pe_schema.txt");
    for (const auto& k : pe::ml::mlFeatureOrder()) sch << k << "\n";
  }
  CHECK(pe::ml::loadModelFromBinary("/tmp/pe_ok.bin", {}, "/tmp/pe_schema.txt").has_value());
  {
    std::ofstream sch("/tmp/pe_schema_bad.txt");
    sch << "wrong_feature\n";
  }
  CHECK(!pe::ml::loadModelFromBinary("/tmp/pe_ok.bin", {}, "/tmp/pe_schema_bad.txt").has_value());
  CHECK(!pe::ml::featureSchemaHash().empty());

  if (failures) { std::cerr << failures << " fails\n"; return 1; }
  std::cout << "ml_model_loader_test OK\n";
  return 0;
}
