#pragma once
#include <functional>
#include <string>
#include <vector>
namespace crashcore {
struct WalkForwardFold {
  std::size_t trainStart=0, trainEnd=0, testStart=0, testEnd=0;
  double metric=0;
};
class WalkForwardEngine {
public:
  std::vector<WalkForwardFold> split(std::size_t n, std::size_t nFolds=5, double testFrac=0.2) const {
    std::vector<WalkForwardFold> folds;
    if (n < nFolds*2) return folds;
    const std::size_t testSize = std::max<std::size_t>(1, std::size_t(n * testFrac / nFolds));
    for (std::size_t f=0; f<nFolds; ++f) {
      WalkForwardFold fold;
      fold.testEnd = n - f*testSize;
      fold.testStart = fold.testEnd > testSize ? fold.testEnd - testSize : 0;
      fold.trainStart = 0;
      fold.trainEnd = fold.testStart;
      if (fold.trainEnd > fold.trainStart) folds.push_back(fold);
    }
    return folds;
  }
  template <typename TrainFn, typename TestFn>
  double run(std::size_t n, TrainFn train, TestFn test, std::size_t nFolds=5) {
    auto folds = split(n, nFolds);
    double sum=0; int c=0;
    for (auto& f : folds) {
      train(f.trainStart, f.trainEnd);
      f.metric = test(f.testStart, f.testEnd);
      sum += f.metric; ++c;
    }
    return c? sum/c : 0;
  }
};
} // namespace crashcore
