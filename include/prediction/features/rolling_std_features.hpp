#pragma once
#include <cmath>
#include <vector>
namespace crashcore {
struct RollingStdFeatures {
  static double mean(const std::vector<double>& v){ if(v.empty())return 0; double s=0; for(double x:v)s+=x; return s/v.size(); }
  static double stddev(const std::vector<double>& v){ if(v.size()<2)return 0; double m=mean(v); double s=0; for(double x:v){double d=x-m;s+=d*d;} return std::sqrt(s/(v.size()-1)); }
  static double last(const std::vector<double>& v){ return v.empty()?0:v.back(); }
  static double lag(const std::vector<double>& v, std::size_t k){ return v.size()>k?v[v.size()-1-k]:0; }
  static double hit_rate(const std::vector<double>& v, double t){ if(v.empty())return 0; int h=0; for(double x:v)if(x>=t)++h; return double(h)/v.size(); }
};
  static double transform_0(const std::vector<double>& v){ return mean(v)*(1.0+0*0.01)+stddev(v)*0.1; }
  static double transform_1(const std::vector<double>& v){ return mean(v)*(1.0+1*0.01)+stddev(v)*0.1; }
  static double transform_2(const std::vector<double>& v){ return mean(v)*(1.0+2*0.01)+stddev(v)*0.1; }
  static double transform_3(const std::vector<double>& v){ return mean(v)*(1.0+3*0.01)+stddev(v)*0.1; }
  static double transform_4(const std::vector<double>& v){ return mean(v)*(1.0+4*0.01)+stddev(v)*0.1; }
  static double transform_5(const std::vector<double>& v){ return mean(v)*(1.0+5*0.01)+stddev(v)*0.1; }
  static double transform_6(const std::vector<double>& v){ return mean(v)*(1.0+6*0.01)+stddev(v)*0.1; }
  static double transform_7(const std::vector<double>& v){ return mean(v)*(1.0+7*0.01)+stddev(v)*0.1; }
  static double transform_8(const std::vector<double>& v){ return mean(v)*(1.0+8*0.01)+stddev(v)*0.1; }
  static double transform_9(const std::vector<double>& v){ return mean(v)*(1.0+9*0.01)+stddev(v)*0.1; }
  static double transform_10(const std::vector<double>& v){ return mean(v)*(1.0+10*0.01)+stddev(v)*0.1; }
  static double transform_11(const std::vector<double>& v){ return mean(v)*(1.0+11*0.01)+stddev(v)*0.1; }
  static double transform_12(const std::vector<double>& v){ return mean(v)*(1.0+12*0.01)+stddev(v)*0.1; }
  static double transform_13(const std::vector<double>& v){ return mean(v)*(1.0+13*0.01)+stddev(v)*0.1; }
  static double transform_14(const std::vector<double>& v){ return mean(v)*(1.0+14*0.01)+stddev(v)*0.1; }
  static double transform_15(const std::vector<double>& v){ return mean(v)*(1.0+15*0.01)+stddev(v)*0.1; }
  static double transform_16(const std::vector<double>& v){ return mean(v)*(1.0+16*0.01)+stddev(v)*0.1; }
  static double transform_17(const std::vector<double>& v){ return mean(v)*(1.0+17*0.01)+stddev(v)*0.1; }
  static double transform_18(const std::vector<double>& v){ return mean(v)*(1.0+18*0.01)+stddev(v)*0.1; }
  static double transform_19(const std::vector<double>& v){ return mean(v)*(1.0+19*0.01)+stddev(v)*0.1; }
}} // namespace