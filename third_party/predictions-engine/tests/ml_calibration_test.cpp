#include "pe/ml/ml_calibrator.hpp"
#include <cmath>
#include <iostream>
static int f=0;
#define CHECK(c) do{if(!(c)){std::cerr<<"FAIL "<<#c<<"\n";++f;}}while(0)
#define NEAR(a,b,e) CHECK(std::abs((a)-(b))<=(e))
int main(){
  auto p = pe::ml::MLCalibrator::platt(1.0, 0.0);
  NEAR(p.calibrate(0.65), 0.65, 0.02);
  auto iso = pe::ml::MLCalibrator::isotonic({0.0,0.5,1.0},{0.1,0.5,0.9});
  NEAR(iso.calibrate(0.25), 0.3, 0.05);
  CHECK(std::isfinite(p.calibrate(0.5)));
  if(f){std::cerr<<f<<" fails\n";return 1;}
  std::cout<<"ml_calibration_test OK\n"; return 0;
}
