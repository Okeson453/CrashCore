#include "pe/ml/ml_predictor.hpp"
#include "pe/ml/ml_runtime.hpp"
#include "pe/features/feature_engine_v2.hpp"
#include "pe/state/incremental_state_engine.hpp"
#include "pe/util/latency_budget.hpp"
#include <chrono>
#include <iostream>
static int f=0;
#define CHECK(c) do{if(!(c)){std::cerr<<"FAIL "<<#c<<"\n";++f;}}while(0)
int main(){
  pe::ml::MLRuntime rt; rt.loadDefault();
  pe::IncrementalStateEngine eng;
  for(int i=0;i<80;++i) eng.update(1.2+(i%3)*0.2);
  pe::FeatureEngineV2 fe; auto fv=fe.compute(eng,"L");
  pe::ml::MLPredictor p(&rt);
  for(int i=0;i<20;++i) p.predict(fv);
  double max_us=0;
  for(int i=0;i<300;++i){
    auto t0=std::chrono::steady_clock::now();
    auto r=p.predict(fv);
    auto t1=std::chrono::steady_clock::now();
    double us=std::chrono::duration<double,std::micro>(t1-t0).count();
    if(us>max_us) max_us=us;
    CHECK(r.has_value());
  }
  CHECK(max_us < static_cast<double>(pe::LATENCY_BUDGET_US));
  std::cout<<"ml_latency_test OK max_us="<<max_us<<"\n";
  if(f){std::cerr<<f<<" fails\n";return 1;}
  return 0;
}
