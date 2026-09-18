#include <limits>
#include <array>
#include "pe/ml/ml_preprocessor.hpp"
#include "pe/ml/ml_feature_vector.hpp"
#include "pe/types.hpp"
#include <cmath>
#include <iostream>
static int f=0;
#define CHECK(c) do{if(!(c)){std::cerr<<"FAIL "<<#c<<"\n";++f;}}while(0)
int main(){
  pe::FeatureVector fv; fv.values["ewma_hit_13"]=0.7;
  std::array<double,pe::ml::MAX_ML_FEATURES> buf{}; std::size_t n=0;
  CHECK(pe::ml::preprocess(fv,buf.data(),buf.size(),&n));
  CHECK(n==pe::ml::mlFeatureOrder().size());
  fv.values["ewma_hit_13"]=std::numeric_limits<double>::quiet_NaN();
  CHECK(!pe::ml::preprocess(fv,buf.data(),buf.size(),&n));
  if(f){std::cerr<<f<<" fails\n";return 1;}
  std::cout<<"ml_preprocessor_test OK\n"; return 0;
}
