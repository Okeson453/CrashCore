# CrashCore

C++23 production runtime for BC.Game crash ingestion, prediction-engine interface,
validation, outbox delivery, and observability.

**Prediction Engine is intentionally separate (EXCLUDE).**

## Production LOC

See measured count after build. Target ≥15,000 production LOC.

## TestingEngine source audit (summary)

| Classification | Examples |
|----------------|----------|
| PORT_PROTOCOL | native-protocol.ts → socketio_protocol, protocol_edge_cases |
| PORT_BEHAVIOR | native-socket-client, realtime pipeline, live validator, outbox, telegram format |
| PORT_CONTRACT | prediction types, prediction-attempt, feedback, target-coordinator |
| REFERENCE_ONLY | predictor.ts, poll-worker, fetch-bc |
| EXCLUDE | ACIE, models, features, ensemble, prediction-engine internals |

Full table: `include/audit/source_mapping.hpp` (48 entries).

## Architecture

```
BC.Game → NativeBcGameSocket → Decoder → RealtimePipeline
                                         → RoundState → EventRouter
                                              ↓
                                    PredictionAttempt (external PE)
                                              ↓
                                    LiveValidator → Feedback → Outbox → Telegram
                                              ↓
                                         Persistence (async)
```

## Event loop (PipelineLoop / EventEngine)

1. Frame classify (ping/pong/open/binary/malformed)
2. Decode binary → CrashEvent
3. Realtime normalize + validate
4. Duplicate filter
5. Round state machine
6. Route to prediction / validation / persistence queues
7. Prediction attempt coordinator (fence + timeout)
8. onGameEnd → outcomes + loss cooldown
9. Outbox lifecycle (claim/lease/wake)
10. Optional Telegram dispatcher + DB batch

## Quick test

```bash
c++ -std=c++23 -Iinclude tests/test_main.cpp -o crashcore_tests -lssl -lcrypto -pthread
./crashcore_tests
```

## Latency (measured)

- decode+route ≈ 0.5–0.8 µs/op
- e2e 1000 rounds ≈ 7–11 µs/round
