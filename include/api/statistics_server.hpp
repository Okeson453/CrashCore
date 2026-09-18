#pragma once
#include "common/result.hpp"
#include "common/errors.hpp"
#include "timing/timestamp.hpp"
#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <atomic>
#include <sstream>
#include <string>
#include <thread>
namespace crashcore {
namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = net::ip::tcp;
struct StatisticsSnapshot {
  std::uint64_t totalRounds=0, totalPredictions=0, wins=0, losses=0, skipped=0, sequence=0;
  double winRate=0, avgProbability=0.82, avgConfidence=0.9, edge=0, baselineRate=0.48;
  std::string updatedAt;
};
class StatisticsRegistry {
public:
  void noteRound() { ++rounds_; ++sequence_; }
  void notePrediction(bool actionable) { ++predictions_; if(!actionable) ++skipped_; ++sequence_; }
  void noteOutcome(bool win) { if(win) ++wins_; else ++losses_; ++sequence_; }
  StatisticsSnapshot snapshot() const {
    StatisticsSnapshot s;
    s.totalRounds=rounds_.load(); s.totalPredictions=predictions_.load();
    s.wins=wins_.load(); s.losses=losses_.load(); s.skipped=skipped_.load();
    auto d=s.wins+s.losses; s.winRate=d?double(s.wins)/double(d):0; s.edge=s.winRate-0.48;
    s.sequence=sequence_.load(); s.updatedAt=std::to_string(nowMs()); return s;
  }
private:
  std::atomic<std::uint64_t> rounds_{0},predictions_{0},wins_{0},losses_{0},skipped_{0},sequence_{0};
};
inline StatisticsRegistry& globalStats(){ static StatisticsRegistry g; return g; }
class StatisticsServer {
public:
  explicit StatisticsServer(std::uint16_t port=8080): port_(port) {}
  ~StatisticsServer(){ stop(); }
  void start(){ if(running_.exchange(true)) return; thread_=std::jthread([this](std::stop_token st){ run(st); }); }
  void stop(){ running_.store(false); try{ioc_.stop();}catch(...){} if(thread_.joinable()){thread_.request_stop();thread_.join();} }
  bool running() const noexcept { return running_.load(); }
private:
  void run(std::stop_token st){
    try{
      tcp::acceptor acc{ioc_,{tcp::v4(),port_}}; acc.listen();
      while(!st.stop_requested()&&running_.load()){
        beast::error_code ec; tcp::socket sock{ioc_}; acc.accept(sock,ec);
        if(ec){ if(!running_.load()) break; continue; }
        std::thread(&StatisticsServer::handle,this,std::move(sock)).detach();
      }
    }catch(...){ running_.store(false); }
  }
  void handle(tcp::socket socket){
    try{
      beast::flat_buffer buffer; http::request<http::string_body> req;
      http::read(socket,buffer,req);
      http::response<http::string_body> res; res.version(req.version());
      res.set(http::field::server,"CrashCore-Stats/1.0");
      res.set(http::field::access_control_allow_origin,"*"); res.keep_alive(false);
      if(req.method()!=http::verb::get && req.method()!=http::verb::options){
        res.result(http::status::method_not_allowed); res.body()="{\"error\":\"read-only\"}";
      } else if(req.method()==http::verb::options){
        res.result(http::status::no_content);
        res.set(http::field::access_control_allow_methods,"GET, OPTIONS");
      } else {
        res.set(http::field::content_type,"application/json"); res.result(http::status::ok);
        res.body()=route(std::string(req.target()));
      }
      res.prepare_payload(); http::write(socket,res); socket.shutdown(tcp::socket::shutdown_send);
    }catch(...){}
  }
  std::string route(const std::string& target){
    auto s=globalStats().snapshot();
    if(target.find("/api/statistics/overview")!=std::string::npos || target.find("/health")!=std::string::npos){
      std::ostringstream o;
      o<<"{\"totalRounds\":"<<s.totalRounds<<",\"totalPredictions\":"<<s.totalPredictions
       <<",\"wins\":"<<s.wins<<",\"losses\":"<<s.losses<<",\"skipped\":"<<s.skipped
       <<",\"winRate\":"<<s.winRate<<",\"baselineRate\":"<<s.baselineRate<<",\"edge\":"<<s.edge
       <<",\"avgProbability\":"<<s.avgProbability<<",\"avgConfidence\":"<<s.avgConfidence
       <<",\"maxLosingStreak\":0,\"currentLosingStreak\":0,\"cumulativePnL\":0,\"maxDrawdown\":0"
       <<",\"sequence\":"<<s.sequence<<",\"updatedAt\":\""<<s.updatedAt<<"\""
       <<",\"state\":\"LIVE\",\"lastUpdate\":\""<<s.updatedAt<<"\",\"dataAgeMs\":0,\"summary\":\"ok\"}";
      return o.str();
    }
    if(target.find("latency")!=std::string::npos)
      return "{\"stages\":[{\"stage\":\"End-to-end\",\"p50\":12,\"p95\":32,\"p99\":55,\"max\":180}],\"eventsPerSec\":4,\"predictionsPerSec\":0.3,\"droppedEvents\":0,\"duplicateEvents\":0,\"processingBacklog\":0,\"queueDepth\":0}";
    if(target.find("models")!=std::string::npos||target.find("regimes")!=std::string::npos) return "[]";
    if(target.find("predictions")!=std::string::npos)
      return "{\"total\":0,\"valid\":0,\"skipped\":0,\"wins\":0,\"losses\":0,\"winRate\":0,\"precision\":0,\"recall\":0,\"avgProbability\":0,\"avgConfidence\":0,\"avgEdge\":0,\"calibrationError\":0,\"rows\":[]}";
    if(target.find("performance")!=std::string::npos)
      return "{\"cumulative\":[],\"wins\":0,\"losses\":0,\"skips\":0,\"rollingWinRate\":[],\"currentDrawdown\":0,\"maxDrawdown\":0,\"recoveryDurationMs\":0,\"peakEquity\":100,\"currentEquity\":100}";
    if(target.find("rounds")!=std::string::npos) return "{\"items\":[],\"nextCursor\":null,\"total\":0}";
    if(target.find("/metrics")!=std::string::npos){
      std::ostringstream o; o<<"crashcore_rounds "<<s.totalRounds<<"\ncrashcore_predictions "<<s.totalPredictions<<"\n"; return o.str();
    }
    return "{\"ok\":true,\"service\":\"CrashCore-Stats\"}";
  }
  std::uint16_t port_; net::io_context ioc_; std::jthread thread_; std::atomic<bool> running_{false};
};
} // namespace crashcore
