#include "pe/ml/ml_fusion.hpp"
#include <cmath>
#include <iostream>
static int f=0;
#define CHECK(c) do{if(!(c)){std::cerr<<"FAIL "<<#c<<"\n";++f;}}while(0)
#define NEAR(a,b,e) CHECK(std::abs((a)-(b))<=(e))
int main(){
  auto f0 = pe::ml::MLFusion::fixedWeight(0.0);
  pe::ml::FusionInput in{0.7,0.4,0.8,0.5,0,0.5};
  NEAR(f0.fuse(in).probability, 0.7, 0.01);
  auto f25 = pe::ml::MLFusion::fixedWeight(0.25);
  NEAR(f25.fuse(in).probability, 0.7*0.75+0.4*0.25, 0.02);
  if(f){std::cerr<<f<<" fails\n";return 1;}
  std::cout<<"ml_fusion_test OK\n"; return 0;
}
