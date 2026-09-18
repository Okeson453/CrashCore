#pragma once
/**
 * Scenario definitions for offline pipeline validation.
 * Each scenario encodes a sequence of CrashEvents derived from TE live flows.
 */
#include "common/types.hpp"
#include "ingestion/crash_event.hpp"
#include "ingestion/socketio/protocol_edge_cases.hpp"
#include "timing/timestamp.hpp"
#include <string>
#include <vector>
#include <functional>

namespace crashcore::scenarios {

struct ScenarioStep {
  std::string name;
  std::vector<std::uint8_t> frame;
  EventKind expectedKind = EventKind::Unknown;
};

struct Scenario {
  std::string id;
  std::string description;
  std::vector<ScenarioStep> steps;
  double expectedFinalMult = 0;
};

inline ScenarioStep stepProgress(std::int64_t elapsedMs) {
  ScenarioStep s;
  s.name = "pg_" + std::to_string(elapsedMs);
  s.frame = encodeEvent("/g/cm", "pg", encodeProgressPayload(elapsedMs));
  s.expectedKind = EventKind::Progress;
  return s;
}

inline ScenarioStep stepStart(std::uint64_t gameId) {
  ScenarioStep s;
  s.name = "st_" + std::to_string(gameId);
  s.frame = encodeEvent("/g/cm", "st", encodeEndPayload(gameId, 1.0));
  s.expectedKind = EventKind::Start;
  return s;
}

inline ScenarioStep stepEnd(std::uint64_t gameId, double mult, std::string hash = "s") {
  ScenarioStep s;
  s.name = "ed_" + std::to_string(gameId);
  s.frame = encodeEvent("/g/cm", "ed", encodeEndPayload(gameId, mult, hash));
  s.expectedKind = EventKind::End;
  return s;
}

inline Scenario makeRoundScenario(std::uint64_t gameId, double finalMult, int ticks = 5) {
  Scenario sc;
  sc.id = "round_" + std::to_string(gameId);
  sc.description = "full round to " + std::to_string(finalMult);
  sc.expectedFinalMult = finalMult;
  sc.steps.push_back(stepStart(gameId));
  auto total = elapsedFromMultiplier(finalMult);
  for (int i = 1; i <= ticks; ++i)
    sc.steps.push_back(stepProgress(total * i / ticks));
  sc.steps.push_back(stepEnd(gameId, finalMult));
  return sc;
}

inline Scenario scenario_001() {
  return makeRoundScenario(30001ull, 1.35, 4);
}
inline Scenario scenario_002() {
  return makeRoundScenario(30002ull, 1.60, 5);
}
inline Scenario scenario_003() {
  return makeRoundScenario(30003ull, 1.85, 6);
}
inline Scenario scenario_004() {
  return makeRoundScenario(30004ull, 2.10, 7);
}
inline Scenario scenario_005() {
  return makeRoundScenario(30005ull, 2.35, 8);
}
inline Scenario scenario_006() {
  return makeRoundScenario(30006ull, 2.60, 9);
}
inline Scenario scenario_007() {
  return makeRoundScenario(30007ull, 2.85, 3);
}
inline Scenario scenario_008() {
  return makeRoundScenario(30008ull, 3.10, 4);
}
inline Scenario scenario_009() {
  return makeRoundScenario(30009ull, 3.35, 5);
}
inline Scenario scenario_010() {
  return makeRoundScenario(30010ull, 3.60, 6);
}
inline Scenario scenario_011() {
  return makeRoundScenario(30011ull, 3.85, 7);
}
inline Scenario scenario_012() {
  return makeRoundScenario(30012ull, 4.10, 8);
}
inline Scenario scenario_013() {
  return makeRoundScenario(30013ull, 4.35, 9);
}
inline Scenario scenario_014() {
  return makeRoundScenario(30014ull, 4.60, 3);
}
inline Scenario scenario_015() {
  return makeRoundScenario(30015ull, 4.85, 4);
}
inline Scenario scenario_016() {
  return makeRoundScenario(30016ull, 5.10, 5);
}
inline Scenario scenario_017() {
  return makeRoundScenario(30017ull, 5.35, 6);
}
inline Scenario scenario_018() {
  return makeRoundScenario(30018ull, 5.60, 7);
}
inline Scenario scenario_019() {
  return makeRoundScenario(30019ull, 5.85, 8);
}
inline Scenario scenario_020() {
  return makeRoundScenario(30020ull, 6.10, 9);
}
inline Scenario scenario_021() {
  return makeRoundScenario(30021ull, 6.35, 3);
}
inline Scenario scenario_022() {
  return makeRoundScenario(30022ull, 6.60, 4);
}
inline Scenario scenario_023() {
  return makeRoundScenario(30023ull, 6.85, 5);
}
inline Scenario scenario_024() {
  return makeRoundScenario(30024ull, 7.10, 6);
}
inline Scenario scenario_025() {
  return makeRoundScenario(30025ull, 7.35, 7);
}
inline Scenario scenario_026() {
  return makeRoundScenario(30026ull, 7.60, 8);
}
inline Scenario scenario_027() {
  return makeRoundScenario(30027ull, 7.85, 9);
}
inline Scenario scenario_028() {
  return makeRoundScenario(30028ull, 8.10, 3);
}
inline Scenario scenario_029() {
  return makeRoundScenario(30029ull, 8.35, 4);
}
inline Scenario scenario_030() {
  return makeRoundScenario(30030ull, 8.60, 5);
}
inline Scenario scenario_031() {
  return makeRoundScenario(30031ull, 8.85, 6);
}
inline Scenario scenario_032() {
  return makeRoundScenario(30032ull, 9.10, 7);
}
inline Scenario scenario_033() {
  return makeRoundScenario(30033ull, 9.35, 8);
}
inline Scenario scenario_034() {
  return makeRoundScenario(30034ull, 9.60, 9);
}
inline Scenario scenario_035() {
  return makeRoundScenario(30035ull, 9.85, 3);
}
inline Scenario scenario_036() {
  return makeRoundScenario(30036ull, 10.10, 4);
}
inline Scenario scenario_037() {
  return makeRoundScenario(30037ull, 10.35, 5);
}
inline Scenario scenario_038() {
  return makeRoundScenario(30038ull, 10.60, 6);
}
inline Scenario scenario_039() {
  return makeRoundScenario(30039ull, 10.85, 7);
}
inline Scenario scenario_040() {
  return makeRoundScenario(30040ull, 1.10, 8);
}
inline Scenario scenario_041() {
  return makeRoundScenario(30041ull, 1.35, 9);
}
inline Scenario scenario_042() {
  return makeRoundScenario(30042ull, 1.60, 3);
}
inline Scenario scenario_043() {
  return makeRoundScenario(30043ull, 1.85, 4);
}
inline Scenario scenario_044() {
  return makeRoundScenario(30044ull, 2.10, 5);
}
inline Scenario scenario_045() {
  return makeRoundScenario(30045ull, 2.35, 6);
}
inline Scenario scenario_046() {
  return makeRoundScenario(30046ull, 2.60, 7);
}
inline Scenario scenario_047() {
  return makeRoundScenario(30047ull, 2.85, 8);
}
inline Scenario scenario_048() {
  return makeRoundScenario(30048ull, 3.10, 9);
}
inline Scenario scenario_049() {
  return makeRoundScenario(30049ull, 3.35, 3);
}
inline Scenario scenario_050() {
  return makeRoundScenario(30050ull, 3.60, 4);
}
inline Scenario scenario_051() {
  return makeRoundScenario(30051ull, 3.85, 5);
}
inline Scenario scenario_052() {
  return makeRoundScenario(30052ull, 4.10, 6);
}
inline Scenario scenario_053() {
  return makeRoundScenario(30053ull, 4.35, 7);
}
inline Scenario scenario_054() {
  return makeRoundScenario(30054ull, 4.60, 8);
}
inline Scenario scenario_055() {
  return makeRoundScenario(30055ull, 4.85, 9);
}
inline Scenario scenario_056() {
  return makeRoundScenario(30056ull, 5.10, 3);
}
inline Scenario scenario_057() {
  return makeRoundScenario(30057ull, 5.35, 4);
}
inline Scenario scenario_058() {
  return makeRoundScenario(30058ull, 5.60, 5);
}
inline Scenario scenario_059() {
  return makeRoundScenario(30059ull, 5.85, 6);
}
inline Scenario scenario_060() {
  return makeRoundScenario(30060ull, 6.10, 7);
}
inline Scenario scenario_061() {
  return makeRoundScenario(30061ull, 6.35, 8);
}
inline Scenario scenario_062() {
  return makeRoundScenario(30062ull, 6.60, 9);
}
inline Scenario scenario_063() {
  return makeRoundScenario(30063ull, 6.85, 3);
}
inline Scenario scenario_064() {
  return makeRoundScenario(30064ull, 7.10, 4);
}
inline Scenario scenario_065() {
  return makeRoundScenario(30065ull, 7.35, 5);
}
inline Scenario scenario_066() {
  return makeRoundScenario(30066ull, 7.60, 6);
}
inline Scenario scenario_067() {
  return makeRoundScenario(30067ull, 7.85, 7);
}
inline Scenario scenario_068() {
  return makeRoundScenario(30068ull, 8.10, 8);
}
inline Scenario scenario_069() {
  return makeRoundScenario(30069ull, 8.35, 9);
}
inline Scenario scenario_070() {
  return makeRoundScenario(30070ull, 8.60, 3);
}
inline Scenario scenario_071() {
  return makeRoundScenario(30071ull, 8.85, 4);
}
inline Scenario scenario_072() {
  return makeRoundScenario(30072ull, 9.10, 5);
}
inline Scenario scenario_073() {
  return makeRoundScenario(30073ull, 9.35, 6);
}
inline Scenario scenario_074() {
  return makeRoundScenario(30074ull, 9.60, 7);
}
inline Scenario scenario_075() {
  return makeRoundScenario(30075ull, 9.85, 8);
}
inline Scenario scenario_076() {
  return makeRoundScenario(30076ull, 10.10, 9);
}
inline Scenario scenario_077() {
  return makeRoundScenario(30077ull, 10.35, 3);
}
inline Scenario scenario_078() {
  return makeRoundScenario(30078ull, 10.60, 4);
}
inline Scenario scenario_079() {
  return makeRoundScenario(30079ull, 10.85, 5);
}
inline Scenario scenario_080() {
  return makeRoundScenario(30080ull, 1.10, 6);
}
inline Scenario scenario_081() {
  return makeRoundScenario(30081ull, 1.35, 7);
}
inline Scenario scenario_082() {
  return makeRoundScenario(30082ull, 1.60, 8);
}
inline Scenario scenario_083() {
  return makeRoundScenario(30083ull, 1.85, 9);
}
inline Scenario scenario_084() {
  return makeRoundScenario(30084ull, 2.10, 3);
}
inline Scenario scenario_085() {
  return makeRoundScenario(30085ull, 2.35, 4);
}
inline Scenario scenario_086() {
  return makeRoundScenario(30086ull, 2.60, 5);
}
inline Scenario scenario_087() {
  return makeRoundScenario(30087ull, 2.85, 6);
}
inline Scenario scenario_088() {
  return makeRoundScenario(30088ull, 3.10, 7);
}
inline Scenario scenario_089() {
  return makeRoundScenario(30089ull, 3.35, 8);
}
inline Scenario scenario_090() {
  return makeRoundScenario(30090ull, 3.60, 9);
}
inline Scenario scenario_091() {
  return makeRoundScenario(30091ull, 3.85, 3);
}
inline Scenario scenario_092() {
  return makeRoundScenario(30092ull, 4.10, 4);
}
inline Scenario scenario_093() {
  return makeRoundScenario(30093ull, 4.35, 5);
}
inline Scenario scenario_094() {
  return makeRoundScenario(30094ull, 4.60, 6);
}
inline Scenario scenario_095() {
  return makeRoundScenario(30095ull, 4.85, 7);
}
inline Scenario scenario_096() {
  return makeRoundScenario(30096ull, 5.10, 8);
}
inline Scenario scenario_097() {
  return makeRoundScenario(30097ull, 5.35, 9);
}
inline Scenario scenario_098() {
  return makeRoundScenario(30098ull, 5.60, 3);
}
inline Scenario scenario_099() {
  return makeRoundScenario(30099ull, 5.85, 4);
}
inline Scenario scenario_100() {
  return makeRoundScenario(30100ull, 6.10, 5);
}
inline Scenario scenario_101() {
  return makeRoundScenario(30101ull, 6.35, 6);
}
inline Scenario scenario_102() {
  return makeRoundScenario(30102ull, 6.60, 7);
}
inline Scenario scenario_103() {
  return makeRoundScenario(30103ull, 6.85, 8);
}
inline Scenario scenario_104() {
  return makeRoundScenario(30104ull, 7.10, 9);
}
inline Scenario scenario_105() {
  return makeRoundScenario(30105ull, 7.35, 3);
}
inline Scenario scenario_106() {
  return makeRoundScenario(30106ull, 7.60, 4);
}
inline Scenario scenario_107() {
  return makeRoundScenario(30107ull, 7.85, 5);
}
inline Scenario scenario_108() {
  return makeRoundScenario(30108ull, 8.10, 6);
}
inline Scenario scenario_109() {
  return makeRoundScenario(30109ull, 8.35, 7);
}
inline Scenario scenario_110() {
  return makeRoundScenario(30110ull, 8.60, 8);
}
inline Scenario scenario_111() {
  return makeRoundScenario(30111ull, 8.85, 9);
}
inline Scenario scenario_112() {
  return makeRoundScenario(30112ull, 9.10, 3);
}
inline Scenario scenario_113() {
  return makeRoundScenario(30113ull, 9.35, 4);
}
inline Scenario scenario_114() {
  return makeRoundScenario(30114ull, 9.60, 5);
}
inline Scenario scenario_115() {
  return makeRoundScenario(30115ull, 9.85, 6);
}
inline Scenario scenario_116() {
  return makeRoundScenario(30116ull, 10.10, 7);
}
inline Scenario scenario_117() {
  return makeRoundScenario(30117ull, 10.35, 8);
}
inline Scenario scenario_118() {
  return makeRoundScenario(30118ull, 10.60, 9);
}
inline Scenario scenario_119() {
  return makeRoundScenario(30119ull, 10.85, 3);
}
inline Scenario scenario_120() {
  return makeRoundScenario(30120ull, 1.10, 4);
}
inline Scenario scenario_121() {
  return makeRoundScenario(30121ull, 1.35, 5);
}
inline Scenario scenario_122() {
  return makeRoundScenario(30122ull, 1.60, 6);
}
inline Scenario scenario_123() {
  return makeRoundScenario(30123ull, 1.85, 7);
}
inline Scenario scenario_124() {
  return makeRoundScenario(30124ull, 2.10, 8);
}
inline Scenario scenario_125() {
  return makeRoundScenario(30125ull, 2.35, 9);
}
inline Scenario scenario_126() {
  return makeRoundScenario(30126ull, 2.60, 3);
}
inline Scenario scenario_127() {
  return makeRoundScenario(30127ull, 2.85, 4);
}
inline Scenario scenario_128() {
  return makeRoundScenario(30128ull, 3.10, 5);
}
inline Scenario scenario_129() {
  return makeRoundScenario(30129ull, 3.35, 6);
}
inline Scenario scenario_130() {
  return makeRoundScenario(30130ull, 3.60, 7);
}
inline Scenario scenario_131() {
  return makeRoundScenario(30131ull, 3.85, 8);
}
inline Scenario scenario_132() {
  return makeRoundScenario(30132ull, 4.10, 9);
}
inline Scenario scenario_133() {
  return makeRoundScenario(30133ull, 4.35, 3);
}
inline Scenario scenario_134() {
  return makeRoundScenario(30134ull, 4.60, 4);
}
inline Scenario scenario_135() {
  return makeRoundScenario(30135ull, 4.85, 5);
}
inline Scenario scenario_136() {
  return makeRoundScenario(30136ull, 5.10, 6);
}
inline Scenario scenario_137() {
  return makeRoundScenario(30137ull, 5.35, 7);
}
inline Scenario scenario_138() {
  return makeRoundScenario(30138ull, 5.60, 8);
}
inline Scenario scenario_139() {
  return makeRoundScenario(30139ull, 5.85, 9);
}
inline Scenario scenario_140() {
  return makeRoundScenario(30140ull, 6.10, 3);
}
inline Scenario scenario_141() {
  return makeRoundScenario(30141ull, 6.35, 4);
}
inline Scenario scenario_142() {
  return makeRoundScenario(30142ull, 6.60, 5);
}
inline Scenario scenario_143() {
  return makeRoundScenario(30143ull, 6.85, 6);
}
inline Scenario scenario_144() {
  return makeRoundScenario(30144ull, 7.10, 7);
}
inline Scenario scenario_145() {
  return makeRoundScenario(30145ull, 7.35, 8);
}
inline Scenario scenario_146() {
  return makeRoundScenario(30146ull, 7.60, 9);
}
inline Scenario scenario_147() {
  return makeRoundScenario(30147ull, 7.85, 3);
}
inline Scenario scenario_148() {
  return makeRoundScenario(30148ull, 8.10, 4);
}
inline Scenario scenario_149() {
  return makeRoundScenario(30149ull, 8.35, 5);
}
inline Scenario scenario_150() {
  return makeRoundScenario(30150ull, 8.60, 6);
}

inline std::vector<Scenario> allScenarios() {
  std::vector<Scenario> out;
  out.reserve(150);
  out.push_back(scenario_001());
  out.push_back(scenario_002());
  out.push_back(scenario_003());
  out.push_back(scenario_004());
  out.push_back(scenario_005());
  out.push_back(scenario_006());
  out.push_back(scenario_007());
  out.push_back(scenario_008());
  out.push_back(scenario_009());
  out.push_back(scenario_010());
  out.push_back(scenario_011());
  out.push_back(scenario_012());
  out.push_back(scenario_013());
  out.push_back(scenario_014());
  out.push_back(scenario_015());
  out.push_back(scenario_016());
  out.push_back(scenario_017());
  out.push_back(scenario_018());
  out.push_back(scenario_019());
  out.push_back(scenario_020());
  out.push_back(scenario_021());
  out.push_back(scenario_022());
  out.push_back(scenario_023());
  out.push_back(scenario_024());
  out.push_back(scenario_025());
  out.push_back(scenario_026());
  out.push_back(scenario_027());
  out.push_back(scenario_028());
  out.push_back(scenario_029());
  out.push_back(scenario_030());
  out.push_back(scenario_031());
  out.push_back(scenario_032());
  out.push_back(scenario_033());
  out.push_back(scenario_034());
  out.push_back(scenario_035());
  out.push_back(scenario_036());
  out.push_back(scenario_037());
  out.push_back(scenario_038());
  out.push_back(scenario_039());
  out.push_back(scenario_040());
  out.push_back(scenario_041());
  out.push_back(scenario_042());
  out.push_back(scenario_043());
  out.push_back(scenario_044());
  out.push_back(scenario_045());
  out.push_back(scenario_046());
  out.push_back(scenario_047());
  out.push_back(scenario_048());
  out.push_back(scenario_049());
  out.push_back(scenario_050());
  out.push_back(scenario_051());
  out.push_back(scenario_052());
  out.push_back(scenario_053());
  out.push_back(scenario_054());
  out.push_back(scenario_055());
  out.push_back(scenario_056());
  out.push_back(scenario_057());
  out.push_back(scenario_058());
  out.push_back(scenario_059());
  out.push_back(scenario_060());
  out.push_back(scenario_061());
  out.push_back(scenario_062());
  out.push_back(scenario_063());
  out.push_back(scenario_064());
  out.push_back(scenario_065());
  out.push_back(scenario_066());
  out.push_back(scenario_067());
  out.push_back(scenario_068());
  out.push_back(scenario_069());
  out.push_back(scenario_070());
  out.push_back(scenario_071());
  out.push_back(scenario_072());
  out.push_back(scenario_073());
  out.push_back(scenario_074());
  out.push_back(scenario_075());
  out.push_back(scenario_076());
  out.push_back(scenario_077());
  out.push_back(scenario_078());
  out.push_back(scenario_079());
  out.push_back(scenario_080());
  out.push_back(scenario_081());
  out.push_back(scenario_082());
  out.push_back(scenario_083());
  out.push_back(scenario_084());
  out.push_back(scenario_085());
  out.push_back(scenario_086());
  out.push_back(scenario_087());
  out.push_back(scenario_088());
  out.push_back(scenario_089());
  out.push_back(scenario_090());
  out.push_back(scenario_091());
  out.push_back(scenario_092());
  out.push_back(scenario_093());
  out.push_back(scenario_094());
  out.push_back(scenario_095());
  out.push_back(scenario_096());
  out.push_back(scenario_097());
  out.push_back(scenario_098());
  out.push_back(scenario_099());
  out.push_back(scenario_100());
  out.push_back(scenario_101());
  out.push_back(scenario_102());
  out.push_back(scenario_103());
  out.push_back(scenario_104());
  out.push_back(scenario_105());
  out.push_back(scenario_106());
  out.push_back(scenario_107());
  out.push_back(scenario_108());
  out.push_back(scenario_109());
  out.push_back(scenario_110());
  out.push_back(scenario_111());
  out.push_back(scenario_112());
  out.push_back(scenario_113());
  out.push_back(scenario_114());
  out.push_back(scenario_115());
  out.push_back(scenario_116());
  out.push_back(scenario_117());
  out.push_back(scenario_118());
  out.push_back(scenario_119());
  out.push_back(scenario_120());
  out.push_back(scenario_121());
  out.push_back(scenario_122());
  out.push_back(scenario_123());
  out.push_back(scenario_124());
  out.push_back(scenario_125());
  out.push_back(scenario_126());
  out.push_back(scenario_127());
  out.push_back(scenario_128());
  out.push_back(scenario_129());
  out.push_back(scenario_130());
  out.push_back(scenario_131());
  out.push_back(scenario_132());
  out.push_back(scenario_133());
  out.push_back(scenario_134());
  out.push_back(scenario_135());
  out.push_back(scenario_136());
  out.push_back(scenario_137());
  out.push_back(scenario_138());
  out.push_back(scenario_139());
  out.push_back(scenario_140());
  out.push_back(scenario_141());
  out.push_back(scenario_142());
  out.push_back(scenario_143());
  out.push_back(scenario_144());
  out.push_back(scenario_145());
  out.push_back(scenario_146());
  out.push_back(scenario_147());
  out.push_back(scenario_148());
  out.push_back(scenario_149());
  out.push_back(scenario_150());
  return out;
}

inline int scenarioCount() { return 150; }

} // namespace crashcore::scenarios
