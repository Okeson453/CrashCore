#pragma once
/**
 * Large fixture library for protocol + pipeline tests.
 * Frames synthesized from TestingEngine native-protocol semantics.
 */
#include "ingestion/socketio/socketio_protocol.hpp"
#include "ingestion/socketio/protocol_edge_cases.hpp"
#include "ingestion/crash_event.hpp"
#include "common/types.hpp"
#include <vector>
#include <string>
#include <cstdint>

namespace crashcore::fixtures {

struct RoundSpec {
  std::uint64_t gameId = 1;
  double finalMult = 2.0;
  int progressTicks = 10;
  std::string hash = "fixture";
};

inline std::vector<std::vector<std::uint8_t>> makeRound(const RoundSpec& s) {
  return syntheticRoundFrames(s.gameId, s.finalMult, s.progressTicks);
}

inline std::vector<std::vector<std::uint8_t>> makeManyRounds(int n, double baseMult = 1.5) {
  std::vector<std::vector<std::uint8_t>> all;
  for (int i = 0; i < n; ++i) {
    RoundSpec s;
    s.gameId = static_cast<std::uint64_t>(1000 + i);
    s.finalMult = baseMult + (i % 20) * 0.15;
    s.progressTicks = 5 + (i % 5);
    auto frames = makeRound(s);
    all.insert(all.end(), frames.begin(), frames.end());
  }
  return all;
}


inline std::vector<std::vector<std::uint8_t>> fixture_round_001() {
  RoundSpec s;
  s.gameId = 10001;
  s.finalMult = 1.10;
  s.progressTicks = 4;
  s.hash = "h1";
  return makeRound(s);
}

inline std::vector<std::vector<std::uint8_t>> fixture_round_002() {
  RoundSpec s;
  s.gameId = 10002;
  s.finalMult = 1.20;
  s.progressTicks = 5;
  s.hash = "h2";
  return makeRound(s);
}

inline std::vector<std::vector<std::uint8_t>> fixture_round_003() {
  RoundSpec s;
  s.gameId = 10003;
  s.finalMult = 1.30;
  s.progressTicks = 6;
  s.hash = "h3";
  return makeRound(s);
}

inline std::vector<std::vector<std::uint8_t>> fixture_round_004() {
  RoundSpec s;
  s.gameId = 10004;
  s.finalMult = 1.40;
  s.progressTicks = 7;
  s.hash = "h4";
  return makeRound(s);
}

inline std::vector<std::vector<std::uint8_t>> fixture_round_005() {
  RoundSpec s;
  s.gameId = 10005;
  s.finalMult = 1.50;
  s.progressTicks = 8;
  s.hash = "h5";
  return makeRound(s);
}

inline std::vector<std::vector<std::uint8_t>> fixture_round_006() {
  RoundSpec s;
  s.gameId = 10006;
  s.finalMult = 1.60;
  s.progressTicks = 9;
  s.hash = "h6";
  return makeRound(s);
}

inline std::vector<std::vector<std::uint8_t>> fixture_round_007() {
  RoundSpec s;
  s.gameId = 10007;
  s.finalMult = 1.70;
  s.progressTicks = 10;
  s.hash = "h7";
  return makeRound(s);
}

inline std::vector<std::vector<std::uint8_t>> fixture_round_008() {
  RoundSpec s;
  s.gameId = 10008;
  s.finalMult = 1.80;
  s.progressTicks = 3;
  s.hash = "h8";
  return makeRound(s);
}

inline std::vector<std::vector<std::uint8_t>> fixture_round_009() {
  RoundSpec s;
  s.gameId = 10009;
  s.finalMult = 1.90;
  s.progressTicks = 4;
  s.hash = "h9";
  return makeRound(s);
}

inline std::vector<std::vector<std::uint8_t>> fixture_round_010() {
  RoundSpec s;
  s.gameId = 10010;
  s.finalMult = 2.00;
  s.progressTicks = 5;
  s.hash = "h10";
  return makeRound(s);
}

inline std::vector<std::vector<std::uint8_t>> fixture_round_011() {
  RoundSpec s;
  s.gameId = 10011;
  s.finalMult = 2.10;
  s.progressTicks = 6;
  s.hash = "h11";
  return makeRound(s);
}

inline std::vector<std::vector<std::uint8_t>> fixture_round_012() {
  RoundSpec s;
  s.gameId = 10012;
  s.finalMult = 2.20;
  s.progressTicks = 7;
  s.hash = "h12";
  return makeRound(s);
}

inline std::vector<std::vector<std::uint8_t>> fixture_round_013() {
  RoundSpec s;
  s.gameId = 10013;
  s.finalMult = 2.30;
  s.progressTicks = 8;
  s.hash = "h13";
  return makeRound(s);
}

inline std::vector<std::vector<std::uint8_t>> fixture_round_014() {
  RoundSpec s;
  s.gameId = 10014;
  s.finalMult = 2.40;
  s.progressTicks = 9;
  s.hash = "h14";
  return makeRound(s);
}

inline std::vector<std::vector<std::uint8_t>> fixture_round_015() {
  RoundSpec s;
  s.gameId = 10015;
  s.finalMult = 2.50;
  s.progressTicks = 10;
  s.hash = "h15";
  return makeRound(s);
}

inline std::vector<std::vector<std::uint8_t>> fixture_round_016() {
  RoundSpec s;
  s.gameId = 10016;
  s.finalMult = 2.60;
  s.progressTicks = 3;
  s.hash = "h16";
  return makeRound(s);
}

inline std::vector<std::vector<std::uint8_t>> fixture_round_017() {
  RoundSpec s;
  s.gameId = 10017;
  s.finalMult = 2.70;
  s.progressTicks = 4;
  s.hash = "h17";
  return makeRound(s);
}

inline std::vector<std::vector<std::uint8_t>> fixture_round_018() {
  RoundSpec s;
  s.gameId = 10018;
  s.finalMult = 2.80;
  s.progressTicks = 5;
  s.hash = "h18";
  return makeRound(s);
}

inline std::vector<std::vector<std::uint8_t>> fixture_round_019() {
  RoundSpec s;
  s.gameId = 10019;
  s.finalMult = 2.90;
  s.progressTicks = 6;
  s.hash = "h19";
  return makeRound(s);
}

inline std::vector<std::vector<std::uint8_t>> fixture_round_020() {
  RoundSpec s;
  s.gameId = 10020;
  s.finalMult = 3.00;
  s.progressTicks = 7;
  s.hash = "h20";
  return makeRound(s);
}

inline std::vector<std::vector<std::uint8_t>> fixture_round_021() {
  RoundSpec s;
  s.gameId = 10021;
  s.finalMult = 3.10;
  s.progressTicks = 8;
  s.hash = "h21";
  return makeRound(s);
}

inline std::vector<std::vector<std::uint8_t>> fixture_round_022() {
  RoundSpec s;
  s.gameId = 10022;
  s.finalMult = 3.20;
  s.progressTicks = 9;
  s.hash = "h22";
  return makeRound(s);
}

inline std::vector<std::vector<std::uint8_t>> fixture_round_023() {
  RoundSpec s;
  s.gameId = 10023;
  s.finalMult = 3.30;
  s.progressTicks = 10;
  s.hash = "h23";
  return makeRound(s);
}

inline std::vector<std::vector<std::uint8_t>> fixture_round_024() {
  RoundSpec s;
  s.gameId = 10024;
  s.finalMult = 3.40;
  s.progressTicks = 3;
  s.hash = "h24";
  return makeRound(s);
}

inline std::vector<std::vector<std::uint8_t>> fixture_round_025() {
  RoundSpec s;
  s.gameId = 10025;
  s.finalMult = 3.50;
  s.progressTicks = 4;
  s.hash = "h25";
  return makeRound(s);
}

inline std::vector<std::vector<std::uint8_t>> fixture_round_026() {
  RoundSpec s;
  s.gameId = 10026;
  s.finalMult = 3.60;
  s.progressTicks = 5;
  s.hash = "h26";
  return makeRound(s);
}

inline std::vector<std::vector<std::uint8_t>> fixture_round_027() {
  RoundSpec s;
  s.gameId = 10027;
  s.finalMult = 3.70;
  s.progressTicks = 6;
  s.hash = "h27";
  return makeRound(s);
}

inline std::vector<std::vector<std::uint8_t>> fixture_round_028() {
  RoundSpec s;
  s.gameId = 10028;
  s.finalMult = 3.80;
  s.progressTicks = 7;
  s.hash = "h28";
  return makeRound(s);
}

inline std::vector<std::vector<std::uint8_t>> fixture_round_029() {
  RoundSpec s;
  s.gameId = 10029;
  s.finalMult = 3.90;
  s.progressTicks = 8;
  s.hash = "h29";
  return makeRound(s);
}

inline std::vector<std::vector<std::uint8_t>> fixture_round_030() {
  RoundSpec s;
  s.gameId = 10030;
  s.finalMult = 4.00;
  s.progressTicks = 9;
  s.hash = "h30";
  return makeRound(s);
}

inline std::vector<std::vector<std::uint8_t>> fixture_round_031() {
  RoundSpec s;
  s.gameId = 10031;
  s.finalMult = 4.10;
  s.progressTicks = 10;
  s.hash = "h31";
  return makeRound(s);
}

inline std::vector<std::vector<std::uint8_t>> fixture_round_032() {
  RoundSpec s;
  s.gameId = 10032;
  s.finalMult = 4.20;
  s.progressTicks = 3;
  s.hash = "h32";
  return makeRound(s);
}

inline std::vector<std::vector<std::uint8_t>> fixture_round_033() {
  RoundSpec s;
  s.gameId = 10033;
  s.finalMult = 4.30;
  s.progressTicks = 4;
  s.hash = "h33";
  return makeRound(s);
}

inline std::vector<std::vector<std::uint8_t>> fixture_round_034() {
  RoundSpec s;
  s.gameId = 10034;
  s.finalMult = 4.40;
  s.progressTicks = 5;
  s.hash = "h34";
  return makeRound(s);
}

inline std::vector<std::vector<std::uint8_t>> fixture_round_035() {
  RoundSpec s;
  s.gameId = 10035;
  s.finalMult = 4.50;
  s.progressTicks = 6;
  s.hash = "h35";
  return makeRound(s);
}

inline std::vector<std::vector<std::uint8_t>> fixture_round_036() {
  RoundSpec s;
  s.gameId = 10036;
  s.finalMult = 4.60;
  s.progressTicks = 7;
  s.hash = "h36";
  return makeRound(s);
}

inline std::vector<std::vector<std::uint8_t>> fixture_round_037() {
  RoundSpec s;
  s.gameId = 10037;
  s.finalMult = 4.70;
  s.progressTicks = 8;
  s.hash = "h37";
  return makeRound(s);
}

inline std::vector<std::vector<std::uint8_t>> fixture_round_038() {
  RoundSpec s;
  s.gameId = 10038;
  s.finalMult = 4.80;
  s.progressTicks = 9;
  s.hash = "h38";
  return makeRound(s);
}

inline std::vector<std::vector<std::uint8_t>> fixture_round_039() {
  RoundSpec s;
  s.gameId = 10039;
  s.finalMult = 4.90;
  s.progressTicks = 10;
  s.hash = "h39";
  return makeRound(s);
}

inline std::vector<std::vector<std::uint8_t>> fixture_round_040() {
  RoundSpec s;
  s.gameId = 10040;
  s.finalMult = 5.00;
  s.progressTicks = 3;
  s.hash = "h40";
  return makeRound(s);
}

inline std::vector<std::vector<std::uint8_t>> fixture_round_041() {
  RoundSpec s;
  s.gameId = 10041;
  s.finalMult = 5.10;
  s.progressTicks = 4;
  s.hash = "h41";
  return makeRound(s);
}

inline std::vector<std::vector<std::uint8_t>> fixture_round_042() {
  RoundSpec s;
  s.gameId = 10042;
  s.finalMult = 5.20;
  s.progressTicks = 5;
  s.hash = "h42";
  return makeRound(s);
}

inline std::vector<std::vector<std::uint8_t>> fixture_round_043() {
  RoundSpec s;
  s.gameId = 10043;
  s.finalMult = 5.30;
  s.progressTicks = 6;
  s.hash = "h43";
  return makeRound(s);
}

inline std::vector<std::vector<std::uint8_t>> fixture_round_044() {
  RoundSpec s;
  s.gameId = 10044;
  s.finalMult = 5.40;
  s.progressTicks = 7;
  s.hash = "h44";
  return makeRound(s);
}

inline std::vector<std::vector<std::uint8_t>> fixture_round_045() {
  RoundSpec s;
  s.gameId = 10045;
  s.finalMult = 5.50;
  s.progressTicks = 8;
  s.hash = "h45";
  return makeRound(s);
}

inline std::vector<std::vector<std::uint8_t>> fixture_round_046() {
  RoundSpec s;
  s.gameId = 10046;
  s.finalMult = 5.60;
  s.progressTicks = 9;
  s.hash = "h46";
  return makeRound(s);
}

inline std::vector<std::vector<std::uint8_t>> fixture_round_047() {
  RoundSpec s;
  s.gameId = 10047;
  s.finalMult = 5.70;
  s.progressTicks = 10;
  s.hash = "h47";
  return makeRound(s);
}

inline std::vector<std::vector<std::uint8_t>> fixture_round_048() {
  RoundSpec s;
  s.gameId = 10048;
  s.finalMult = 5.80;
  s.progressTicks = 3;
  s.hash = "h48";
  return makeRound(s);
}

inline std::vector<std::vector<std::uint8_t>> fixture_round_049() {
  RoundSpec s;
  s.gameId = 10049;
  s.finalMult = 5.90;
  s.progressTicks = 4;
  s.hash = "h49";
  return makeRound(s);
}

inline std::vector<std::vector<std::uint8_t>> fixture_round_050() {
  RoundSpec s;
  s.gameId = 10050;
  s.finalMult = 1.00;
  s.progressTicks = 5;
  s.hash = "h50";
  return makeRound(s);
}

inline std::vector<std::vector<std::uint8_t>> fixture_round_051() {
  RoundSpec s;
  s.gameId = 10051;
  s.finalMult = 1.10;
  s.progressTicks = 6;
  s.hash = "h51";
  return makeRound(s);
}

inline std::vector<std::vector<std::uint8_t>> fixture_round_052() {
  RoundSpec s;
  s.gameId = 10052;
  s.finalMult = 1.20;
  s.progressTicks = 7;
  s.hash = "h52";
  return makeRound(s);
}

inline std::vector<std::vector<std::uint8_t>> fixture_round_053() {
  RoundSpec s;
  s.gameId = 10053;
  s.finalMult = 1.30;
  s.progressTicks = 8;
  s.hash = "h53";
  return makeRound(s);
}

inline std::vector<std::vector<std::uint8_t>> fixture_round_054() {
  RoundSpec s;
  s.gameId = 10054;
  s.finalMult = 1.40;
  s.progressTicks = 9;
  s.hash = "h54";
  return makeRound(s);
}

inline std::vector<std::vector<std::uint8_t>> fixture_round_055() {
  RoundSpec s;
  s.gameId = 10055;
  s.finalMult = 1.50;
  s.progressTicks = 10;
  s.hash = "h55";
  return makeRound(s);
}

inline std::vector<std::vector<std::uint8_t>> fixture_round_056() {
  RoundSpec s;
  s.gameId = 10056;
  s.finalMult = 1.60;
  s.progressTicks = 3;
  s.hash = "h56";
  return makeRound(s);
}

inline std::vector<std::vector<std::uint8_t>> fixture_round_057() {
  RoundSpec s;
  s.gameId = 10057;
  s.finalMult = 1.70;
  s.progressTicks = 4;
  s.hash = "h57";
  return makeRound(s);
}

inline std::vector<std::vector<std::uint8_t>> fixture_round_058() {
  RoundSpec s;
  s.gameId = 10058;
  s.finalMult = 1.80;
  s.progressTicks = 5;
  s.hash = "h58";
  return makeRound(s);
}

inline std::vector<std::vector<std::uint8_t>> fixture_round_059() {
  RoundSpec s;
  s.gameId = 10059;
  s.finalMult = 1.90;
  s.progressTicks = 6;
  s.hash = "h59";
  return makeRound(s);
}

inline std::vector<std::vector<std::uint8_t>> fixture_round_060() {
  RoundSpec s;
  s.gameId = 10060;
  s.finalMult = 2.00;
  s.progressTicks = 7;
  s.hash = "h60";
  return makeRound(s);
}

inline std::vector<std::vector<std::uint8_t>> fixture_round_061() {
  RoundSpec s;
  s.gameId = 10061;
  s.finalMult = 2.10;
  s.progressTicks = 8;
  s.hash = "h61";
  return makeRound(s);
}

inline std::vector<std::vector<std::uint8_t>> fixture_round_062() {
  RoundSpec s;
  s.gameId = 10062;
  s.finalMult = 2.20;
  s.progressTicks = 9;
  s.hash = "h62";
  return makeRound(s);
}

inline std::vector<std::vector<std::uint8_t>> fixture_round_063() {
  RoundSpec s;
  s.gameId = 10063;
  s.finalMult = 2.30;
  s.progressTicks = 10;
  s.hash = "h63";
  return makeRound(s);
}

inline std::vector<std::vector<std::uint8_t>> fixture_round_064() {
  RoundSpec s;
  s.gameId = 10064;
  s.finalMult = 2.40;
  s.progressTicks = 3;
  s.hash = "h64";
  return makeRound(s);
}

inline std::vector<std::vector<std::uint8_t>> fixture_round_065() {
  RoundSpec s;
  s.gameId = 10065;
  s.finalMult = 2.50;
  s.progressTicks = 4;
  s.hash = "h65";
  return makeRound(s);
}

inline std::vector<std::vector<std::uint8_t>> fixture_round_066() {
  RoundSpec s;
  s.gameId = 10066;
  s.finalMult = 2.60;
  s.progressTicks = 5;
  s.hash = "h66";
  return makeRound(s);
}

inline std::vector<std::vector<std::uint8_t>> fixture_round_067() {
  RoundSpec s;
  s.gameId = 10067;
  s.finalMult = 2.70;
  s.progressTicks = 6;
  s.hash = "h67";
  return makeRound(s);
}

inline std::vector<std::vector<std::uint8_t>> fixture_round_068() {
  RoundSpec s;
  s.gameId = 10068;
  s.finalMult = 2.80;
  s.progressTicks = 7;
  s.hash = "h68";
  return makeRound(s);
}

inline std::vector<std::vector<std::uint8_t>> fixture_round_069() {
  RoundSpec s;
  s.gameId = 10069;
  s.finalMult = 2.90;
  s.progressTicks = 8;
  s.hash = "h69";
  return makeRound(s);
}

inline std::vector<std::vector<std::uint8_t>> fixture_round_070() {
  RoundSpec s;
  s.gameId = 10070;
  s.finalMult = 3.00;
  s.progressTicks = 9;
  s.hash = "h70";
  return makeRound(s);
}

inline std::vector<std::vector<std::uint8_t>> fixture_round_071() {
  RoundSpec s;
  s.gameId = 10071;
  s.finalMult = 3.10;
  s.progressTicks = 10;
  s.hash = "h71";
  return makeRound(s);
}

inline std::vector<std::vector<std::uint8_t>> fixture_round_072() {
  RoundSpec s;
  s.gameId = 10072;
  s.finalMult = 3.20;
  s.progressTicks = 3;
  s.hash = "h72";
  return makeRound(s);
}

inline std::vector<std::vector<std::uint8_t>> fixture_round_073() {
  RoundSpec s;
  s.gameId = 10073;
  s.finalMult = 3.30;
  s.progressTicks = 4;
  s.hash = "h73";
  return makeRound(s);
}

inline std::vector<std::vector<std::uint8_t>> fixture_round_074() {
  RoundSpec s;
  s.gameId = 10074;
  s.finalMult = 3.40;
  s.progressTicks = 5;
  s.hash = "h74";
  return makeRound(s);
}

inline std::vector<std::vector<std::uint8_t>> fixture_round_075() {
  RoundSpec s;
  s.gameId = 10075;
  s.finalMult = 3.50;
  s.progressTicks = 6;
  s.hash = "h75";
  return makeRound(s);
}

inline std::vector<std::vector<std::uint8_t>> fixture_round_076() {
  RoundSpec s;
  s.gameId = 10076;
  s.finalMult = 3.60;
  s.progressTicks = 7;
  s.hash = "h76";
  return makeRound(s);
}

inline std::vector<std::vector<std::uint8_t>> fixture_round_077() {
  RoundSpec s;
  s.gameId = 10077;
  s.finalMult = 3.70;
  s.progressTicks = 8;
  s.hash = "h77";
  return makeRound(s);
}

inline std::vector<std::vector<std::uint8_t>> fixture_round_078() {
  RoundSpec s;
  s.gameId = 10078;
  s.finalMult = 3.80;
  s.progressTicks = 9;
  s.hash = "h78";
  return makeRound(s);
}

inline std::vector<std::vector<std::uint8_t>> fixture_round_079() {
  RoundSpec s;
  s.gameId = 10079;
  s.finalMult = 3.90;
  s.progressTicks = 10;
  s.hash = "h79";
  return makeRound(s);
}

inline std::vector<std::vector<std::uint8_t>> fixture_round_080() {
  RoundSpec s;
  s.gameId = 10080;
  s.finalMult = 4.00;
  s.progressTicks = 3;
  s.hash = "h80";
  return makeRound(s);
}

inline std::vector<std::vector<std::uint8_t>> allNamedFixtures() {
  std::vector<std::vector<std::uint8_t>> all;
  { auto f = fixture_round_001(); all.insert(all.end(), f.begin(), f.end()); }
  { auto f = fixture_round_002(); all.insert(all.end(), f.begin(), f.end()); }
  { auto f = fixture_round_003(); all.insert(all.end(), f.begin(), f.end()); }
  { auto f = fixture_round_004(); all.insert(all.end(), f.begin(), f.end()); }
  { auto f = fixture_round_005(); all.insert(all.end(), f.begin(), f.end()); }
  { auto f = fixture_round_006(); all.insert(all.end(), f.begin(), f.end()); }
  { auto f = fixture_round_007(); all.insert(all.end(), f.begin(), f.end()); }
  { auto f = fixture_round_008(); all.insert(all.end(), f.begin(), f.end()); }
  { auto f = fixture_round_009(); all.insert(all.end(), f.begin(), f.end()); }
  { auto f = fixture_round_010(); all.insert(all.end(), f.begin(), f.end()); }
  { auto f = fixture_round_011(); all.insert(all.end(), f.begin(), f.end()); }
  { auto f = fixture_round_012(); all.insert(all.end(), f.begin(), f.end()); }
  { auto f = fixture_round_013(); all.insert(all.end(), f.begin(), f.end()); }
  { auto f = fixture_round_014(); all.insert(all.end(), f.begin(), f.end()); }
  { auto f = fixture_round_015(); all.insert(all.end(), f.begin(), f.end()); }
  { auto f = fixture_round_016(); all.insert(all.end(), f.begin(), f.end()); }
  { auto f = fixture_round_017(); all.insert(all.end(), f.begin(), f.end()); }
  { auto f = fixture_round_018(); all.insert(all.end(), f.begin(), f.end()); }
  { auto f = fixture_round_019(); all.insert(all.end(), f.begin(), f.end()); }
  { auto f = fixture_round_020(); all.insert(all.end(), f.begin(), f.end()); }
  { auto f = fixture_round_021(); all.insert(all.end(), f.begin(), f.end()); }
  { auto f = fixture_round_022(); all.insert(all.end(), f.begin(), f.end()); }
  { auto f = fixture_round_023(); all.insert(all.end(), f.begin(), f.end()); }
  { auto f = fixture_round_024(); all.insert(all.end(), f.begin(), f.end()); }
  { auto f = fixture_round_025(); all.insert(all.end(), f.begin(), f.end()); }
  { auto f = fixture_round_026(); all.insert(all.end(), f.begin(), f.end()); }
  { auto f = fixture_round_027(); all.insert(all.end(), f.begin(), f.end()); }
  { auto f = fixture_round_028(); all.insert(all.end(), f.begin(), f.end()); }
  { auto f = fixture_round_029(); all.insert(all.end(), f.begin(), f.end()); }
  { auto f = fixture_round_030(); all.insert(all.end(), f.begin(), f.end()); }
  { auto f = fixture_round_031(); all.insert(all.end(), f.begin(), f.end()); }
  { auto f = fixture_round_032(); all.insert(all.end(), f.begin(), f.end()); }
  { auto f = fixture_round_033(); all.insert(all.end(), f.begin(), f.end()); }
  { auto f = fixture_round_034(); all.insert(all.end(), f.begin(), f.end()); }
  { auto f = fixture_round_035(); all.insert(all.end(), f.begin(), f.end()); }
  { auto f = fixture_round_036(); all.insert(all.end(), f.begin(), f.end()); }
  { auto f = fixture_round_037(); all.insert(all.end(), f.begin(), f.end()); }
  { auto f = fixture_round_038(); all.insert(all.end(), f.begin(), f.end()); }
  { auto f = fixture_round_039(); all.insert(all.end(), f.begin(), f.end()); }
  { auto f = fixture_round_040(); all.insert(all.end(), f.begin(), f.end()); }
  { auto f = fixture_round_041(); all.insert(all.end(), f.begin(), f.end()); }
  { auto f = fixture_round_042(); all.insert(all.end(), f.begin(), f.end()); }
  { auto f = fixture_round_043(); all.insert(all.end(), f.begin(), f.end()); }
  { auto f = fixture_round_044(); all.insert(all.end(), f.begin(), f.end()); }
  { auto f = fixture_round_045(); all.insert(all.end(), f.begin(), f.end()); }
  { auto f = fixture_round_046(); all.insert(all.end(), f.begin(), f.end()); }
  { auto f = fixture_round_047(); all.insert(all.end(), f.begin(), f.end()); }
  { auto f = fixture_round_048(); all.insert(all.end(), f.begin(), f.end()); }
  { auto f = fixture_round_049(); all.insert(all.end(), f.begin(), f.end()); }
  { auto f = fixture_round_050(); all.insert(all.end(), f.begin(), f.end()); }
  { auto f = fixture_round_051(); all.insert(all.end(), f.begin(), f.end()); }
  { auto f = fixture_round_052(); all.insert(all.end(), f.begin(), f.end()); }
  { auto f = fixture_round_053(); all.insert(all.end(), f.begin(), f.end()); }
  { auto f = fixture_round_054(); all.insert(all.end(), f.begin(), f.end()); }
  { auto f = fixture_round_055(); all.insert(all.end(), f.begin(), f.end()); }
  { auto f = fixture_round_056(); all.insert(all.end(), f.begin(), f.end()); }
  { auto f = fixture_round_057(); all.insert(all.end(), f.begin(), f.end()); }
  { auto f = fixture_round_058(); all.insert(all.end(), f.begin(), f.end()); }
  { auto f = fixture_round_059(); all.insert(all.end(), f.begin(), f.end()); }
  { auto f = fixture_round_060(); all.insert(all.end(), f.begin(), f.end()); }
  { auto f = fixture_round_061(); all.insert(all.end(), f.begin(), f.end()); }
  { auto f = fixture_round_062(); all.insert(all.end(), f.begin(), f.end()); }
  { auto f = fixture_round_063(); all.insert(all.end(), f.begin(), f.end()); }
  { auto f = fixture_round_064(); all.insert(all.end(), f.begin(), f.end()); }
  { auto f = fixture_round_065(); all.insert(all.end(), f.begin(), f.end()); }
  { auto f = fixture_round_066(); all.insert(all.end(), f.begin(), f.end()); }
  { auto f = fixture_round_067(); all.insert(all.end(), f.begin(), f.end()); }
  { auto f = fixture_round_068(); all.insert(all.end(), f.begin(), f.end()); }
  { auto f = fixture_round_069(); all.insert(all.end(), f.begin(), f.end()); }
  { auto f = fixture_round_070(); all.insert(all.end(), f.begin(), f.end()); }
  { auto f = fixture_round_071(); all.insert(all.end(), f.begin(), f.end()); }
  { auto f = fixture_round_072(); all.insert(all.end(), f.begin(), f.end()); }
  { auto f = fixture_round_073(); all.insert(all.end(), f.begin(), f.end()); }
  { auto f = fixture_round_074(); all.insert(all.end(), f.begin(), f.end()); }
  { auto f = fixture_round_075(); all.insert(all.end(), f.begin(), f.end()); }
  { auto f = fixture_round_076(); all.insert(all.end(), f.begin(), f.end()); }
  { auto f = fixture_round_077(); all.insert(all.end(), f.begin(), f.end()); }
  { auto f = fixture_round_078(); all.insert(all.end(), f.begin(), f.end()); }
  { auto f = fixture_round_079(); all.insert(all.end(), f.begin(), f.end()); }
  { auto f = fixture_round_080(); all.insert(all.end(), f.begin(), f.end()); }
  return all;
}

inline int countFixtures() { return 80; }

} // namespace crashcore::fixtures
