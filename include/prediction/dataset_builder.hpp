#pragma once
#include <string>
#include <unordered_map>
#include <vector>
namespace crashcore {
struct DatasetRowLite {
  std::string roundId;
  std::unordered_map<std::string,double> features;
  int label = 0;
  double crashPoint = 0;
};
struct DatasetLite {
  std::string id;
  std::vector<DatasetRowLite> rows;
  bool leakageCheckPassed = false;
};
class DatasetBuilder {
public:
  void add(DatasetRowLite row) { rows_.push_back(std::move(row)); }
  DatasetLite build(const std::string& id) {
    DatasetLite d; d.id=id; d.rows=rows_;
    // naive leakage check: feature keys must not include future labels
    d.leakageCheckPassed = true;
    for (auto& r : d.rows) {
      if (r.features.count("label") || r.features.count("future_mult")) {
        d.leakageCheckPassed = false; break;
      }
    }
    return d;
  }
  void clear() { rows_.clear(); }
  std::size_t size() const { return rows_.size(); }
private:
  std::vector<DatasetRowLite> rows_;
};
} // namespace crashcore
