#pragma once
#include <cstdint>
#include <cstddef>
#include <string_view>

namespace crashcore::constants {

inline constexpr std::size_t  DEFAULT_SPSC_CAPACITY     = 4096;
inline constexpr std::size_t  DEFAULT_MPSC_CAPACITY     = 8192;
inline constexpr std::size_t  DEFAULT_OUTBOX_CAPACITY   = 2048;
inline constexpr std::size_t  DEFAULT_WORKER_COUNT      = 5;

inline constexpr std::int64_t SIGN_FRESH_MS             = 60'000;
inline constexpr std::int64_t SIGN_STALE_MAX_MS         = 600'000;
inline constexpr std::int64_t HEARTBEAT_INTERVAL_MS     = 25'000;
inline constexpr std::int64_t HEARTBEAT_TIMEOUT_MS      = 60'000;
inline constexpr std::int64_t RECONNECT_BASE_MS         = 500;
inline constexpr std::int64_t RECONNECT_MAX_MS          = 30'000;
inline constexpr std::int64_t PREDICTION_TIMEOUT_MS     = 50;
inline constexpr std::int64_t OUTBOX_CLAIM_TIMEOUT_MS   = 30'000;
inline constexpr std::int64_t TELEGRAM_TIMEOUT_MS       = 10'000;
inline constexpr int          TELEGRAM_MAX_RETRIES      = 3;
inline constexpr int          OUTBOX_MAX_ATTEMPTS       = 5;

inline constexpr double       PROGRESS_EXP_COEFF        = 6e-5;
inline constexpr double       MULT_SCALE                = 100.0;

inline constexpr std::string_view BC_GAME_HOST          = "bc.game";
inline constexpr std::string_view BC_CRASH_PATH         = "/game/crash";
inline constexpr std::string_view BC_SOCKET_NSP         = "/g/cm";
inline constexpr std::string_view DEFAULT_UA =
    "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 "
    "(KHTML, like Gecko) Chrome/122.0.0.0 Safari/537.36";

// Latency budgets (application path, excluding network RTT)
inline constexpr std::int64_t BUDGET_PARSE_US           = 500;
inline constexpr std::int64_t BUDGET_STATE_US           = 100;
inline constexpr std::int64_t BUDGET_IPC_US             = 200;
inline constexpr std::int64_t BUDGET_QUEUE_US           = 50;
inline constexpr std::int64_t BUDGET_PREDICTION_US      = 5000;
inline constexpr std::int64_t BUDGET_VALIDATION_US      = 500;
inline constexpr std::int64_t BUDGET_OUTBOX_US          = 1000;
inline constexpr std::int64_t BUDGET_TOTAL_APP_US       = 10000;
inline constexpr std::int64_t BUDGET_E2E_P95_US         = 60000;

} // namespace crashcore::constants
