#pragma once

/**
 * Simple k-means learned clustering for regime labels (8-dim vectors).
 * Ported from learned-clustering.ts — pure loops, no external LA lib.
 */

#include "pe/regimes/regime_state.hpp"

#include <algorithm>
#include <cmath>
#include <string>
#include <vector>

namespace pe {

struct ScalerParams {
  std::string version = "scaler-v1";
  std::vector<double> means;
  std::vector<double> stds;
};

inline ScalerParams fitStandardScaler(const std::vector<std::vector<double>>& rows,
                                      const std::string& version = "scaler-v1") {
  ScalerParams s;
  s.version = version;
  if (rows.empty()) return s;
  const size_t dim = rows[0].size();
  s.means.assign(dim, 0.0);
  s.stds.assign(dim, 1.0);
  for (const auto& r : rows) {
    for (size_t j = 0; j < dim; ++j) s.means[j] += r[j];
  }
  for (size_t j = 0; j < dim; ++j) s.means[j] /= static_cast<double>(rows.size());
  for (const auto& r : rows) {
    for (size_t j = 0; j < dim; ++j) {
      const double d = r[j] - s.means[j];
      s.stds[j] += d * d;
    }
  }
  for (size_t j = 0; j < dim; ++j) {
    s.stds[j] = std::sqrt(s.stds[j] / std::max(1.0, static_cast<double>(rows.size()) - 1.0));
    if (s.stds[j] < 1e-12) s.stds[j] = 1.0;
  }
  return s;
}

inline std::vector<double> transform(const std::vector<double>& row, const ScalerParams& scaler) {
  std::vector<double> out(row.size());
  for (size_t i = 0; i < row.size(); ++i) {
    const double m = i < scaler.means.size() ? scaler.means[i] : 0.0;
    const double s = i < scaler.stds.size() ? scaler.stds[i] : 1.0;
    out[i] = (row[i] - m) / s;
  }
  return out;
}

inline double euclidean(const std::vector<double>& a, const std::vector<double>& b) {
  double sum = 0.0;
  const size_t n = std::min(a.size(), b.size());
  for (size_t i = 0; i < n; ++i) {
    const double d = a[i] - b[i];
    sum += d * d;
  }
  return std::sqrt(sum);
}

struct KMeansResult {
  std::vector<std::vector<double>> centroids;
  std::vector<int> assignments;
};

inline KMeansResult kMeans(const std::vector<std::vector<double>>& rows, int k, int maxIter = 40) {
  KMeansResult result;
  const int n = static_cast<int>(rows.size());
  if (n == 0) return result;
  const size_t dim = rows[0].size();
  const int kk = std::min(k, n);

  result.centroids.resize(static_cast<size_t>(kk));
  for (int i = 0; i < kk; ++i) {
    result.centroids[static_cast<size_t>(i)] = rows[static_cast<size_t>((i * n) / kk)];
  }
  result.assignments.assign(static_cast<size_t>(n), 0);

  for (int iter = 0; iter < maxIter; ++iter) {
    bool changed = false;
    for (int i = 0; i < n; ++i) {
      int best = 0;
      double bestD = 1e300;
      for (int c = 0; c < kk; ++c) {
        const double d = euclidean(rows[static_cast<size_t>(i)],
                                   result.centroids[static_cast<size_t>(c)]);
        if (d < bestD) {
          bestD = d;
          best = c;
        }
      }
      if (result.assignments[static_cast<size_t>(i)] != best) {
        result.assignments[static_cast<size_t>(i)] = best;
        changed = true;
      }
    }

    std::vector<std::vector<double>> sums(static_cast<size_t>(kk), std::vector<double>(dim, 0.0));
    std::vector<int> counts(static_cast<size_t>(kk), 0);
    for (int i = 0; i < n; ++i) {
      const int a = result.assignments[static_cast<size_t>(i)];
      counts[static_cast<size_t>(a)]++;
      for (size_t j = 0; j < dim; ++j)
        sums[static_cast<size_t>(a)][j] += rows[static_cast<size_t>(i)][j];
    }
    for (int c = 0; c < kk; ++c) {
      if (counts[static_cast<size_t>(c)] == 0) continue;
      for (size_t j = 0; j < dim; ++j)
        result.centroids[static_cast<size_t>(c)][j] =
            sums[static_cast<size_t>(c)][j] / counts[static_cast<size_t>(c)];
    }
    if (!changed) break;
  }
  return result;
}

class LearnedRegimeClustering {
public:
  void fit(const std::vector<std::vector<double>>& rows, int k = 8) {
    scaler_ = fitStandardScaler(rows);
    std::vector<std::vector<double>> scaled;
    scaled.reserve(rows.size());
    for (const auto& r : rows) scaled.push_back(transform(r, scaler_));
    auto km = kMeans(scaled, k);
    centroids_ = std::move(km.centroids);
    fitted_ = true;
  }

  RegimeClusterState predict(const std::vector<double>& row) {
    RegimeClusterState st;
    if (!fitted_ || centroids_.empty()) {
      st.label = "unfitted";
      return st;
    }
    const auto scaled = transform(row, scaler_);
    int best = 0;
    double bestD = 1e300;
    for (size_t c = 0; c < centroids_.size(); ++c) {
      const double d = euclidean(scaled, centroids_[c]);
      if (d < bestD) {
        bestD = d;
        best = static_cast<int>(c);
      }
    }
    if (best == lastCluster_) ++duration_;
    else {
      lastCluster_ = best;
      duration_ = 1;
    }
    st.clusterId = best;
    st.clusterDistance = bestD;
    st.clusterConfidence = 1.0 / (1.0 + bestD);
    st.regimeDuration = duration_;
    st.label = "cluster-" + std::to_string(best);
    st.sampleCount = 1;
    return st;
  }

private:
  ScalerParams scaler_;
  std::vector<std::vector<double>> centroids_;
  bool fitted_ = false;
  int lastCluster_ = -1;
  int duration_ = 0;
};

} // namespace pe
