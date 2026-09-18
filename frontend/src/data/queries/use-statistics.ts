"use client";

import { useQuery } from "@tanstack/react-query";
import {
  fetchOverview,
  fetchPredictions,
  fetchPerformance,
  fetchModels,
  fetchRegimes,
  fetchLatency,
  fetchHealth,
  fetchRounds,
} from "@/data/api/statistics";
import type { RoundsFilter } from "@/types/statistics";

export function useOverview() {
  return useQuery({ queryKey: ["overview"], queryFn: fetchOverview, refetchInterval: 15_000, refetchOnReconnect: true });
}
export function usePredictions() {
  return useQuery({ queryKey: ["predictions"], queryFn: fetchPredictions, refetchInterval: 20_000 });
}
export function usePerformance() {
  return useQuery({ queryKey: ["performance"], queryFn: fetchPerformance, refetchInterval: 30_000 });
}
export function useModels() {
  return useQuery({ queryKey: ["models"], queryFn: fetchModels, refetchInterval: 30_000 });
}
export function useRegimes() {
  return useQuery({ queryKey: ["regimes"], queryFn: fetchRegimes, refetchInterval: 30_000 });
}
export function useLatency() {
  return useQuery({ queryKey: ["latency"], queryFn: fetchLatency, refetchInterval: 10_000 });
}
export function useHealth() {
  return useQuery({ queryKey: ["health"], queryFn: fetchHealth, refetchInterval: 5_000 });
}
export function useRounds(filter: RoundsFilter) {
  return useQuery({
    queryKey: ["rounds", filter],
    queryFn: () => fetchRounds(filter),
    refetchInterval: 20_000,
  });
}
