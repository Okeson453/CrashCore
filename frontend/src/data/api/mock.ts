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
  ReliabilityBin,
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
    maxWinningStreak: 23,
    currentWinningStreak: 6,
    cumulativePnL: 214.5,
    maxDrawdown: 0.12,
    sharpeRatio: 2.84,
    profitFactor: 3.42,
    activeModels: 7,
    eventsPerSec: 4.2,
    sequence: 18429,
    updatedAt: new Date().toISOString(),
  };
}

export function mockPredictions(): PredictionStats {
  const rnd = seed(42);
  const models = [
    "Frequency",
    "ConditionalFrequency",
    "RegimeAdjusted",
    "StreakAware",
    "MomentumReversion",
    "Bayesian",
    "VolatilityAdjusted",
  ];
  const regimes = ["High Activity", "Low Activity", "Volatile", "Stable"];
  const rows: PredictionRow[] = Array.from({ length: 50 }, (_, i) => {
    const win = rnd() > 0.1938;
    const target = Number((1.5 + Math.floor(rnd() * 8) * 0.25).toFixed(2));
    const crashMultiplier = win
      ? Number((target + rnd() * 4).toFixed(2))
      : Number((1.0 + rnd() * (target - 1.01)).toFixed(2));
    const prob = Number((0.58 + rnd() * 0.38).toFixed(4));
    return {
      gameId: String(1_580_000 - i),
      timestamp: new Date(Date.now() - i * 45_000).toISOString(),
      target,
      probability: prob,
      confidence: Number((0.72 + rnd() * 0.26).toFixed(4)),
      edge: Number((prob - 0.48).toFixed(4)),
      regime: regimes[Math.floor(rnd() * regimes.length)]!,
      model: models[Math.floor(rnd() * models.length)]!,
      decision: rnd() > 0.12 ? "Bet" : "Skip",
      actualResult: win ? "Win" : "Loss",
      latencyMs: Number((6.2 + rnd() * 32.5).toFixed(2)),
      crashMultiplier,
      brierScore: Number(Math.pow(prob - (win ? 1 : 0), 2).toFixed(4)),
    };
  });

  const reliabilityCurve: ReliabilityBin[] = [
    { bin: "50-60%", predictedAvg: 0.552, observedRate: 0.548, count: 412 },
    { bin: "60-70%", predictedAvg: 0.651, observedRate: 0.664, count: 820 },
    { bin: "70-80%", predictedAvg: 0.753, observedRate: 0.749, count: 1250 },
    { bin: "80-90%", predictedAvg: 0.849, observedRate: 0.858, count: 1140 },
    { bin: "90-100%", predictedAvg: 0.938, observedRate: 0.942, count: 609 },
  ];

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
    reliabilityCurve,
    rows,
  };
}

export function mockPerformance(): PerformanceStats {
  const rnd = seed(7);
  const cumulative: PerformancePoint[] = [];
  let equity = 100;
  let peak = 100;
  for (let i = 0; i < 120; i++) {
    const delta = (rnd() - 0.34) * 3.2;
    equity = Math.max(20, equity + delta);
    peak = Math.max(peak, equity);
    const dd = peak > 0 ? (peak - equity) / peak : 0;
    cumulative.push({
      t: new Date(Date.now() - (120 - i) * 3600_000).toISOString(),
      equity: Number(equity.toFixed(2)),
      winRate: Number((0.68 + rnd() * 0.22).toFixed(4)),
      drawdown: Number(dd.toFixed(4)),
      highWaterMark: Number(peak.toFixed(2)),
      pnlDelta: Number(delta.toFixed(2)),
    });
  }
  return {
    cumulative,
    wins: 3412,
    losses: 819,
    skips: 11647,
    rollingWinRate: [
      { window: 25, rate: 0.82 },
      { window: 50, rate: 0.795 },
      { window: 100, rate: 0.812 },
      { window: 250, rate: 0.805 },
      { window: 500, rate: 0.806 },
    ],
    currentDrawdown: 0.038,
    maxDrawdown: 0.12,
    recoveryDurationMs: 3_600_000,
    peakEquity: Number(peak.toFixed(2)),
    currentEquity: Number(equity.toFixed(2)),
    sharpeRatio: 2.84,
    sortinoRatio: 3.91,
    profitFactor: 3.42,
    avgWinPnL: 1.82,
    avgLossPnL: -1.0,
  };
}

export function mockModels(): ModelStatsRow[] {
  const modelsMeta = [
    {
      model: "Bayesian",
      description: "Conjugate prior-posterior Bayesian updater with online Laplace approximation",
      status: "ACTIVE" as const,
      latency: 3.2,
      brier: 0.024,
    },
    {
      model: "ConditionalFrequency",
      description: "Multi-order lag-conditioned probability transition matrix",
      status: "ACTIVE" as const,
      latency: 2.8,
      brier: 0.029,
    },
    {
      model: "RegimeAdjusted",
      description: "Clustering-stratified classifier adapting to market activity phase",
      status: "ACTIVE" as const,
      latency: 4.1,
      brier: 0.027,
    },
    {
      model: "StreakAware",
      description: "Markov streak duration and run-length distribution estimator",
      status: "ACTIVE" as const,
      latency: 2.4,
      brier: 0.033,
    },
    {
      model: "MomentumReversion",
      description: "Mean-reversion model based on rolling EWMA volatility oscillations",
      status: "ACTIVE" as const,
      latency: 3.6,
      brier: 0.031,
    },
    {
      model: "VolatilityAdjusted",
      description: "GARCH-inspired volatility scaling with extreme outlier rejection",
      status: "ACTIVE" as const,
      latency: 3.9,
      brier: 0.026,
    },
    {
      model: "Frequency",
      description: "Unconditional empirical marginal frequency benchmark baseline",
      status: "ACTIVE" as const,
      latency: 1.5,
      brier: 0.045,
    },
  ];

  const rnd = seed(99);
  return modelsMeta.map((m) => {
    const predictions = 350 + Math.floor(rnd() * 550);
    const winRate = Number((0.74 + rnd() * 0.12).toFixed(4));
    const wins = Math.floor(predictions * winRate);
    const losses = predictions - wins;
    const avgProb = Number((0.76 + rnd() * 0.14).toFixed(4));
    const avgConf = Number((0.86 + rnd() * 0.11).toFixed(4));
    const avgEdge = Number((avgProb - 0.48).toFixed(4));
    return {
      model: m.model,
      description: m.description,
      status: m.status,
      predictions,
      wins,
      losses,
      winRate: Number((wins / predictions).toFixed(4)),
      avgProbability: avgProb,
      avgConfidence: avgConf,
      avgEdge,
      calibrationError: Number((0.018 + rnd() * 0.03).toFixed(4)),
      brierScore: m.brier,
      avgLatencyMs: m.latency,
      maxLosingStreak: 3 + Math.floor(rnd() * 4),
      sampleSize: predictions * 3 + Math.floor(rnd() * 500),
    };
  });
}

export function mockRegimes(): RegimeStatsRow[] {
  const regimesMeta = [
    {
      regime: "High Activity",
      volatilityIndex: 0.88,
      meanMultiplier: 3.42,
      preferredModel: "RegimeAdjusted",
      description: "Elevated transaction density, tight inter-arrival times, rapid round cycling",
    },
    {
      regime: "Stable",
      volatilityIndex: 0.32,
      meanMultiplier: 2.15,
      preferredModel: "Bayesian",
      description: "Low variance multiplier distribution, balanced mean reversion",
    },
    {
      regime: "Volatile",
      volatilityIndex: 0.94,
      meanMultiplier: 4.89,
      preferredModel: "VolatilityAdjusted",
      description: "Wide multiplier dispersion with heavy right-tail outliers",
    },
    {
      regime: "Low Activity",
      volatilityIndex: 0.45,
      meanMultiplier: 2.38,
      preferredModel: "StreakAware",
      description: "Sparse round intervals, clustering in sub-2.00x multiplier bands",
    },
  ];
  const rnd = seed(11);
  return regimesMeta.map((rm) => {
    const predictions = 500 + Math.floor(rnd() * 700);
    const winRate = Number((0.74 + rnd() * 0.11).toFixed(4));
    const wins = Math.floor(predictions * winRate);
    return {
      regime: rm.regime,
      volatilityIndex: rm.volatilityIndex,
      meanMultiplier: rm.meanMultiplier,
      preferredModel: rm.preferredModel,
      description: rm.description,
      samples: predictions * 3 + Math.floor(rnd() * 800),
      predictions,
      wins,
      losses: predictions - wins,
      winRate: Number((wins / predictions).toFixed(4)),
      avgProbability: Number((0.78 + rnd() * 0.11).toFixed(4)),
      avgConfidence: Number((0.89 + rnd() * 0.08).toFixed(4)),
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
