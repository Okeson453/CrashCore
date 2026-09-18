#include <vector>
#include "pe/ml/ml_model_loader.hpp"
#include "pe/ml/ml_inference.hpp"
#include "pe/ml/ml_feature_vector.hpp"
#include <iostream>
static int f=0;
#define CHECK(c) do{if(!(c)){std::cerr<<"FAIL "<<#c<<"\n";++f;}}while(0)
int main(){
  auto loaded = pe::ml::makeDefaultLogisticModel();
  pe::ml::MLInference inf(loaded.model.get());
  std::vector<double> x(loaded.model->featureCount(),0.0);
  const auto& order=pe::ml::mlFeatureOrder();
  for(size_t i=0;i<order.size();++i) if(order[i]=="ewma_hit_13") x[i]=0.8;
  pe::ml::MLFeatureVector fv{x.data(),x.size()};
  auto r = inf.run(fv);
  CHECK(r.valid); CHECK(r.probability>0.01 && r.probability<0.99);
  CHECK(r.inference_us >= 0.0);
  // size mismatch → invalid
  pe::ml::MLFeatureVector bad{x.data(), 1};
  CHECK(!inf.run(bad).valid);
  if(f){std::cerr<<f<<" fails\n";return 1;}
  std::cout<<"ml_inference_test OK\n"; return 0;
}
