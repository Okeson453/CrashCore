/** Read-only statistics types — no prediction or control logic. */

export type ConnectionState = "LIVE" | "DEGRADED" | "STALE" | "DISCONNECTED";

export interface OverviewStats {
  totalRounds: number;
  totalPredictions: number;
  wins: number;
  losses: number;
  skipped: number;
  winRate: number;
  baselineRate: number;
  edge: number;
  avgProbability: number;
  avgConfidence: number;
  maxLosingStreak: number;
  currentLosingStreak: number;
  cumulativePnL: number;
  maxDrawdown: number;
  sequence: number;
  updatedAt: string;
}

export interface PredictionRow {
  gameId: string;
  timestamp: string;
  target: number;
  probability: number;
  confidence: number;
  edge: number;
  regime: string;
  model: string;
  decision: "Bet" | "Skip" | "Void";
  actualResult: "Win" | "Loss" | "Skip" | "Void" | "Pending";
  latencyMs: number;
}

export interface PredictionStats {
  total: number;
  valid: number;
  skipped: number;
  wins: number;
  losses: number;
  winRate: number;
  precision: number;
  recall: number;
  avgProbability: number;
  avgConfidence: number;
  avgEdge: number;
  calibrationError: number;
  rows: PredictionRow[];
}

export interface PerformancePoint {
  t: string;
  equity: number;
  winRate: number;
  drawdown: number;
}

export interface PerformanceStats {
  cumulative: PerformancePoint[];
  wins: number;
  losses: number;
  skips: number;
  rollingWinRate: { window: number; rate: number }[];
  currentDrawdown: number;
  maxDrawdown: number;
  recoveryDurationMs: number;
  peakEquity: number;
  currentEquity: number;
}

export interface ModelStatsRow {
  model: string;
  predictions: number;
  wins: number;
  losses: number;
  winRate: number;
  avgProbability: number;
  avgConfidence: number;
  avgEdge: number;
  calibrationError: number;
  maxLosingStreak: number;
  sampleSize: number;
}

export interface RegimeStatsRow {
  regime: string;
  samples: number;
  predictions: number;
  wins: number;
  losses: number;
  winRate: number;
  avgProbability: number;
  avgConfidence: number;
}

export interface LatencyStage {
  stage: string;
  p50: number;
  p95: number;
  p99: number;
  max: number;
}

export interface LatencyStats {
  stages: LatencyStage[];
  eventsPerSec: number;
  predictionsPerSec: number;
  droppedEvents: number;
  duplicateEvents: number;
  processingBacklog: number;
  queueDepth: number;
}

export interface RoundRow {
  gameId: string;
  timestamp: string;
  multiplier: number;
  hash?: string;
  regime?: string;
}

export interface RoundsPage {
  items: RoundRow[];
  nextCursor: string | null;
  total: number;
}

export interface HealthStatus {
  state: ConnectionState;
  lastUpdate: string;
  sequence: number;
  dataAgeMs: number;
  summary: string;
}

export interface StatisticsUpdate {
  type: "statistics_update";
  timestamp: string;
  rounds: number;
  predictions: number;
  wins: number;
  losses: number;
  winRate: number;
  latencyP95: number;
  sequence: number;
}

export interface RoundsFilter {
  from?: string;
  to?: string;
  regime?: string;
  model?: string;
  result?: string;
  minProbability?: number;
  maxProbability?: number;
  minConfidence?: number;
  maxConfidence?: number;
  cursor?: string;
  limit?: number;
}
