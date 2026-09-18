import type {
  OverviewStats,
  PredictionStats,
  PerformanceStats,
  ModelStatsRow,
  RegimeStatsRow,
  LatencyStats,
  RoundsPage,
  HealthStatus,
  RoundsFilter,
  PredictionRow,
  PerformancePoint,
} from "@/types/statistics";

function seed(n: number) {
  let x = n;
  return () => {
    x = (x * 1664525 + 1013904223) >>> 0;
    return x / 0xffffffff;
  };
}

export function mockOverview(): OverviewStats {
  return {
    totalRounds: 15878,
    totalPredictions: 4231,
    wins: 3412,
    losses: 819,
    skipped: 11647,
    winRate: 0.8062,
    baselineRate: 0.48,
    edge: 0.3262,
    avgProbability: 0.8231,
    avgConfidence: 0.9418,
    maxLosingStreak: 7,
    currentLosingStreak: 1,
    cumulativePnL: 214.5,
    maxDrawdown: 0.12,
    sequence: 18429,
    updatedAt: new Date().toISOString(),
  };
}

export function mockPredictions(): PredictionStats {
  const rnd = seed(42);
  const models = ["Frequency", "ConditionalFrequency", "RegimeAdjusted", "StreakAware", "Bayesian"];
  const regimes = ["High Activity", "Low Activity", "Volatile", "Stable"];
  const rows: PredictionRow[] = Array.from({ length: 40 }, (_, i) => {
    const win = rnd() > 0.2;
    return {
      gameId: String(1_000_000 + i),
      timestamp: new Date(Date.now() - i * 45_000).toISOString(),
      target: 1.5 + Math.floor(rnd() * 8) * 0.25,
      probability: 0.55 + rnd() * 0.4,
      confidence: 0.7 + rnd() * 0.29,
      edge: 0.05 + rnd() * 0.3,
      regime: regimes[Math.floor(rnd() * regimes.length)]!,
      model: models[Math.floor(rnd() * models.length)]!,
      decision: rnd() > 0.15 ? "Bet" : "Skip",
      actualResult: win ? "Win" : "Loss",
      latencyMs: 8 + rnd() * 40,
    };
  });
  return {
    total: 4231,
    valid: 4231,
    skipped: 11647,
    wins: 3412,
    losses: 819,
    winRate: 0.8062,
    precision: 0.812,
    recall: 0.79,
    avgProbability: 0.8231,
    avgConfidence: 0.9418,
    avgEdge: 0.18,
    calibrationError: 0.032,
    rows,
  };
}

export function mockPerformance(): PerformanceStats {
  const rnd = seed(7);
  const cumulative: PerformancePoint[] = [];
  let equity = 100;
  let peak = 100;
  for (let i = 0; i < 120; i++) {
    const delta = (rnd() - 0.35) * 2.5;
    equity = Math.max(20, equity + delta);
    peak = Math.max(peak, equity);
    cumulative.push({
      t: new Date(Date.now() - (120 - i) * 3600_000).toISOString(),
      equity: Number(equity.toFixed(2)),
      winRate: 0.65 + rnd() * 0.25,
      drawdown: peak > 0 ? (peak - equity) / peak : 0,
    });
  }
  return {
    cumulative,
    wins: 3412,
    losses: 819,
    skips: 11647,
    rollingWinRate: [
      { window: 25, rate: 0.8 },
      { window: 50, rate: 0.78 },
      { window: 100, rate: 0.81 },
      { window: 250, rate: 0.805 },
      { window: 500, rate: 0.802 },
    ],
    currentDrawdown: 0.04,
    maxDrawdown: 0.12,
    recoveryDurationMs: 3_600_000,
    peakEquity: peak,
    currentEquity: equity,
  };
}

export function mockModels(): ModelStatsRow[] {
  const names = [
    "Frequency",
    "ConditionalFrequency",
    "RegimeAdjusted",
    "StreakAware",
    "MomentumReversion",
    "Bayesian",
    "VolatilityAdjusted",
  ];
  const rnd = seed(99);
  return names.map((model) => {
    const predictions = 300 + Math.floor(rnd() * 600);
    const wins = Math.floor(predictions * (0.7 + rnd() * 0.15));
    const losses = predictions - wins;
    return {
      model,
      predictions,
      wins,
      losses,
      winRate: wins / predictions,
      avgProbability: 0.75 + rnd() * 0.15,
      avgConfidence: 0.85 + rnd() * 0.12,
      avgEdge: 0.1 + rnd() * 0.2,
      calibrationError: 0.02 + rnd() * 0.04,
      maxLosingStreak: 3 + Math.floor(rnd() * 6),
      sampleSize: predictions,
    };
  });
}

export function mockRegimes(): RegimeStatsRow[] {
  const regimes = ["High Activity", "Low Activity", "Volatile", "Stable"];
  const rnd = seed(11);
  return regimes.map((regime) => {
    const predictions = 400 + Math.floor(rnd() * 800);
    const wins = Math.floor(predictions * (0.72 + rnd() * 0.12));
    return {
      regime,
      samples: predictions * 3,
      predictions,
      wins,
      losses: predictions - wins,
      winRate: wins / predictions,
      avgProbability: 0.78 + rnd() * 0.12,
      avgConfidence: 0.88 + rnd() * 0.1,
    };
  });
}

export function mockLatency(): LatencyStats {
  return {
    stages: [
      { stage: "Event ingestion", p50: 0.4, p95: 1.2, p99: 3.5, max: 12 },
      { stage: "Event normalization", p50: 0.2, p95: 0.6, p99: 1.1, max: 4 },
      { stage: "Feature update", p50: 0.8, p95: 2.1, p99: 5.0, max: 18 },
      { stage: "Prediction compute", p50: 4.5, p95: 12.0, p99: 28, max: 95 },
      { stage: "Publish", p50: 1.0, p95: 3.2, p99: 8, max: 40 },
      { stage: "Persistence", p50: 2.5, p95: 8.0, p99: 20, max: 120 },
      { stage: "End-to-end", p50: 12, p95: 32, p99: 55, max: 180 },
    ],
    eventsPerSec: 4.2,
    predictionsPerSec: 0.35,
    droppedEvents: 12,
    duplicateEvents: 48,
    processingBacklog: 0,
    queueDepth: 3,
  };
}

export function mockRounds(filter: RoundsFilter = {}): RoundsPage {
  const rnd = seed(55);
  const limit = filter.limit ?? 50;
  const items = Array.from({ length: limit }, (_, i) => ({
    gameId: String(1_580_000 - i),
    timestamp: new Date(Date.now() - i * 30_000).toISOString(),
    multiplier: Number((1 + rnd() * 20).toFixed(2)),
    hash: `h${(rnd() * 1e9).toString(16).slice(0, 12)}`,
    regime: ["High Activity", "Low Activity", "Volatile", "Stable"][Math.floor(rnd() * 4)],
  }));
  return { items, nextCursor: String(limit), total: 15878 };
}

export function mockHealth(): HealthStatus {
  return {
    state: "LIVE",
    lastUpdate: new Date().toISOString(),
    sequence: 18429,
    dataAgeMs: 143,
    summary: "healthy socket=up outbox_pending=0",
  };
}
