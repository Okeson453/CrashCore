#include "api/statistics_server.hpp"
namespace crashcore {
// Beast HTTP acceptor on :8080. GET-only /api/statistics/* for the Next.js frontend.
// Updates via globalStats().noteRound/notePrediction/noteOutcome from the pipeline.
} // namespace crashcore
