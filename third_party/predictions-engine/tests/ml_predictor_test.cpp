#include "pe/ml/ml_predictor.hpp"
#include "pe/ml/ml_runtime.hpp"
#include "pe/features/feature_engine_v2.hpp"
#include "pe/state/incremental_state_engine.hpp"
#include <iostream>
static int f=0;
#define CHECK(c) do{if(!(c)){std::cerr<<"FAIL "<<#c<<"\n";++f;}}while(0)
int main(){
  pe::ml::MLRuntime rt; CHECK(rt.loadDefault());
  pe::IncrementalStateEngine eng;
  for(int i=0;i<50;++i) eng.update(1.1+(i%4)*0.3);
  pe::FeatureEngineV2 fe; auto fv=fe.compute(eng,"t");
  pe::ml::MLPredictor p(&rt);
  auto r=p.predict(fv); CHECK(r.has_value()); CHECK(r->valid);
  pe::ml::MLPredictor dead(nullptr); CHECK(!dead.predict(fv).has_value());
  if(f){std::cerr<<f<<" fails\n";return 1;}
  std::cout<<"ml_predictor_test OK\n"; return 0;
}
