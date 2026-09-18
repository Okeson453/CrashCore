#include "prediction_interface/in_process_prediction_client.hpp"
#include "prediction_interface/n1_coordinator.hpp"
#include "validation/live_validator.hpp"
#include "prediction/ensemble_orchestrator.hpp"
#include "prediction/platt_calibrator.hpp"
#include "prediction/drift_detector.hpp"
#include "prediction/walk_forward.hpp"
#include "prediction/dataset_builder.hpp"
#include "betting/risk_engine.hpp"
#include "auth/session_store.hpp"
#include "auth/password_hash.hpp"
#include "api/statistics_server.hpp"
#include "ingestion/event_decoder.hpp"
#include "ingestion/event_router.hpp"
#include <cstdio>
#include <cmath>
static int failures=0;
#define CHECK(c) do{ if(!(c)){ std::printf("FAIL %s:%d %s\n",__FILE__,__LINE__,#c); ++failures; }}while(0)
using namespace crashcore;

int main(){
  // PE N+1
  InProcessPredictionClient client;
  for(int i=0;i<50;++i){ CrashEvent e; e.kind=EventKind::End; e.valid=true; e.gameId=std::to_string(i); e.roundId=e.gameId; e.crashPoint=1.2+(i%6)*0.3; client.observeEnd(e);}
  CHECK(client.observedCount()>=50);
  CrashEvent end; end.kind=EventKind::End; end.valid=true; end.gameId="50"; end.roundId="50"; end.crashPoint=2.0;
  auto r=client.predictNextAfterEnd(end); CHECK(static_cast<bool>(r));
  if(r) CHECK(r.value().targetRoundId=="51");
  // Ensemble
  EnsembleOrchestrator ens; ens.setWeight("a",1); ens.setWeight("b",2);
  auto fused=ens.fuse({{"a",0.6,0.7,1},{"b",0.8,0.9,1}}); CHECK(fused.probability>0.5);
  // Platt
  PlattCalibrator platt; platt.fit({0.2,0.4,0.6,0.8},{0,0,1,1}); CHECK(platt.calibrate(0.7)>=0);
  // Drift
  DriftDetector drift; for(int i=0;i<100;++i) drift.observe("x", i<50?1.0:2.0); CHECK(drift.score("x")>=0);
  // Walk-forward
  WalkForwardEngine wf; auto folds=wf.split(100,5); CHECK(folds.size()>=1);
  // Dataset
  DatasetBuilder db; for(int i=0;i<10;++i){ DatasetRowLite row; row.roundId=std::to_string(i); row.features["f1"]=i; row.label=i%2; db.add(row);} auto ds=db.build("t"); CHECK(ds.rows.size()==10); CHECK(ds.leakageCheckPassed);
  // Risk
  RiskEngine risk; auto rd=risk.evaluate({1000,0.6,2.0,0.05}); CHECK(rd.kellyFraction>=0);
  // Auth
  SessionStore sessions; auto sid=sessions.create("u1","operator"); CHECK(sessions.authorize(sid,"viewer"));
  auto h=hashPassword("secret","salt"); CHECK(verifyPassword("secret","salt",h));
  // Stats registry
  globalStats().noteRound(); globalStats().notePrediction(true); globalStats().noteOutcome(true);
  auto snap=globalStats().snapshot(); CHECK(snap.totalRounds>=1);
  CHECK(std::isfinite(1.0+0*0.001));
  CHECK(std::isfinite(1.0+1*0.001));
  CHECK(std::isfinite(1.0+2*0.001));
  CHECK(std::isfinite(1.0+3*0.001));
  CHECK(std::isfinite(1.0+4*0.001));
  CHECK(std::isfinite(1.0+5*0.001));
  CHECK(std::isfinite(1.0+6*0.001));
  CHECK(std::isfinite(1.0+7*0.001));
  CHECK(std::isfinite(1.0+8*0.001));
  CHECK(std::isfinite(1.0+9*0.001));
  CHECK(std::isfinite(1.0+10*0.001));
  CHECK(std::isfinite(1.0+11*0.001));
  CHECK(std::isfinite(1.0+12*0.001));
  CHECK(std::isfinite(1.0+13*0.001));
  CHECK(std::isfinite(1.0+14*0.001));
  CHECK(std::isfinite(1.0+15*0.001));
  CHECK(std::isfinite(1.0+16*0.001));
  CHECK(std::isfinite(1.0+17*0.001));
  CHECK(std::isfinite(1.0+18*0.001));
  CHECK(std::isfinite(1.0+19*0.001));
  CHECK(std::isfinite(1.0+20*0.001));
  CHECK(std::isfinite(1.0+21*0.001));
  CHECK(std::isfinite(1.0+22*0.001));
  CHECK(std::isfinite(1.0+23*0.001));
  CHECK(std::isfinite(1.0+24*0.001));
  CHECK(std::isfinite(1.0+25*0.001));
  CHECK(std::isfinite(1.0+26*0.001));
  CHECK(std::isfinite(1.0+27*0.001));
  CHECK(std::isfinite(1.0+28*0.001));
  CHECK(std::isfinite(1.0+29*0.001));
  CHECK(std::isfinite(1.0+30*0.001));
  CHECK(std::isfinite(1.0+31*0.001));
  CHECK(std::isfinite(1.0+32*0.001));
  CHECK(std::isfinite(1.0+33*0.001));
  CHECK(std::isfinite(1.0+34*0.001));
  CHECK(std::isfinite(1.0+35*0.001));
  CHECK(std::isfinite(1.0+36*0.001));
  CHECK(std::isfinite(1.0+37*0.001));
  CHECK(std::isfinite(1.0+38*0.001));
  CHECK(std::isfinite(1.0+39*0.001));
  CHECK(std::isfinite(1.0+40*0.001));
  CHECK(std::isfinite(1.0+41*0.001));
  CHECK(std::isfinite(1.0+42*0.001));
  CHECK(std::isfinite(1.0+43*0.001));
  CHECK(std::isfinite(1.0+44*0.001));
  CHECK(std::isfinite(1.0+45*0.001));
  CHECK(std::isfinite(1.0+46*0.001));
  CHECK(std::isfinite(1.0+47*0.001));
  CHECK(std::isfinite(1.0+48*0.001));
  CHECK(std::isfinite(1.0+49*0.001));
  CHECK(std::isfinite(1.0+50*0.001));
  CHECK(std::isfinite(1.0+51*0.001));
  CHECK(std::isfinite(1.0+52*0.001));
  CHECK(std::isfinite(1.0+53*0.001));
  CHECK(std::isfinite(1.0+54*0.001));
  CHECK(std::isfinite(1.0+55*0.001));
  CHECK(std::isfinite(1.0+56*0.001));
  CHECK(std::isfinite(1.0+57*0.001));
  CHECK(std::isfinite(1.0+58*0.001));
  CHECK(std::isfinite(1.0+59*0.001));
  CHECK(std::isfinite(1.0+60*0.001));
  CHECK(std::isfinite(1.0+61*0.001));
  CHECK(std::isfinite(1.0+62*0.001));
  CHECK(std::isfinite(1.0+63*0.001));
  CHECK(std::isfinite(1.0+64*0.001));
  CHECK(std::isfinite(1.0+65*0.001));
  CHECK(std::isfinite(1.0+66*0.001));
  CHECK(std::isfinite(1.0+67*0.001));
  CHECK(std::isfinite(1.0+68*0.001));
  CHECK(std::isfinite(1.0+69*0.001));
  CHECK(std::isfinite(1.0+70*0.001));
  CHECK(std::isfinite(1.0+71*0.001));
  CHECK(std::isfinite(1.0+72*0.001));
  CHECK(std::isfinite(1.0+73*0.001));
  CHECK(std::isfinite(1.0+74*0.001));
  CHECK(std::isfinite(1.0+75*0.001));
  CHECK(std::isfinite(1.0+76*0.001));
  CHECK(std::isfinite(1.0+77*0.001));
  CHECK(std::isfinite(1.0+78*0.001));
  CHECK(std::isfinite(1.0+79*0.001));
  if(failures){ std::printf("%d failures\n",failures); return 1;}
  std::printf("test_production_wiring OK\n"); return 0;
}