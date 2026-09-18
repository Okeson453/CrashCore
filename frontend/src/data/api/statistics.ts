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
} from "@/types/statistics";
import {
  mockOverview,
  mockPredictions,
  mockPerformance,
  mockModels,
  mockRegimes,
  mockLatency,
  mockRounds,
  mockHealth,
} from "./mock";

const BASE = "/api/statistics";

/** True when last successful response came from CrashCore backend (not demo fallback). */
let backendReachable = false;
let lastError: string | null = null;

export function isBackendReachable(): boolean {
  return backendReachable;
}
export function getLastApiError(): string | null {
  return lastError;
}

/**
 * Fetch JSON from statistics API.
 * - On success: marks backend reachable, returns live data.
 * - On failure: throws unless NEXT_PUBLIC_ALLOW_DEMO_FALLBACK === "true"
 *   (explicit opt-in only — production must not silent-mock).
 * - Never pretends mock data is live.
 */
function demoFallbackAllowed(): boolean {
  return process.env.NEXT_PUBLIC_ALLOW_DEMO_FALLBACK === "true";
}

async function getJson<T>(path: string, fallback: T): Promise<T> {
  try {
    const res = await fetch(path, {
      method: "GET",
      headers: { Accept: "application/json" },
      cache: "no-store",
    });
    if (!res.ok) {
      lastError = `HTTP ${res.status} ${path}`;
      backendReachable = false;
      if (!demoFallbackAllowed()) {
        throw new Error(lastError);
      }
      return fallback;
    }
    const data = (await res.json()) as T;
    const any = data as Record<string, unknown>;
    if (any && any.__source === "demo") {
      backendReachable = false;
      lastError = "backend offline (demo payload)";
      if (!demoFallbackAllowed()) {
        throw new Error(lastError);
      }
      return fallback;
    }
    backendReachable = true;
    lastError = null;
    return data;
  } catch (e) {
    lastError = e instanceof Error ? e.message : String(e);
    backendReachable = false;
    if (!demoFallbackAllowed()) {
      throw e instanceof Error ? e : new Error(lastError);
    }
    return fallback;
  }
}

export function fetchOverview(): Promise<OverviewStats> {
  return getJson(`${BASE}/overview`, mockOverview());
}
export function fetchPredictions(): Promise<PredictionStats> {
  return getJson(`${BASE}/predictions`, mockPredictions());
}
export function fetchPerformance(): Promise<PerformanceStats> {
  return getJson(`${BASE}/performance`, mockPerformance());
}
export function fetchModels(): Promise<ModelStatsRow[]> {
  return getJson(`${BASE}/models`, mockModels());
}
export function fetchRegimes(): Promise<RegimeStatsRow[]> {
  return getJson(`${BASE}/regimes`, mockRegimes());
}
export function fetchLatency(): Promise<LatencyStats> {
  return getJson(`${BASE}/latency`, mockLatency());
}
export function fetchHealth(): Promise<HealthStatus> {
  return getJson(`${BASE}/health`, mockHealth());
}
export function fetchRounds(filter: RoundsFilter = {}): Promise<RoundsPage> {
  const q = new URLSearchParams();
  if (filter.from) q.set("from", filter.from);
  if (filter.to) q.set("to", filter.to);
  if (filter.regime) q.set("regime", filter.regime);
  if (filter.model) q.set("model", filter.model);
  if (filter.result) q.set("result", filter.result);
  if (filter.minProbability != null) q.set("minProbability", String(filter.minProbability));
  if (filter.maxProbability != null) q.set("maxProbability", String(filter.maxProbability));
  if (filter.minConfidence != null) q.set("minConfidence", String(filter.minConfidence));
  if (filter.maxConfidence != null) q.set("maxConfidence", String(filter.maxConfidence));
  if (filter.cursor) q.set("cursor", filter.cursor);
  if (filter.limit) q.set("limit", String(filter.limit));
  const qs = q.toString();
  return getJson(`${BASE}/rounds${qs ? `?${qs}` : ""}`, mockRounds(filter));
}

/** Explicit demo-only helpers for offline development. */
export const demo = {
  overview: mockOverview,
  predictions: mockPredictions,
  performance: mockPerformance,
  models: mockModels,
  regimes: mockRegimes,
  latency: mockLatency,
  rounds: mockRounds,
  health: mockHealth,
};
