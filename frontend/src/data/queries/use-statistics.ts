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
import { useStatisticsStore } from "@/stores/statistics-store";

export function useOverview() {
  const interval = useStatisticsStore((s) => s.autoRefreshInterval);
  return useQuery({
    queryKey: ["overview"],
    queryFn: fetchOverview,
    refetchInterval: interval > 0 ? interval : false,
    refetchOnReconnect: true,
  });
}

export function usePredictions() {
  const interval = useStatisticsStore((s) => s.autoRefreshInterval);
  return useQuery({
    queryKey: ["predictions"],
    queryFn: fetchPredictions,
    refetchInterval: interval > 0 ? Math.max(interval, 10_000) : false,
  });
}

export function usePerformance() {
  const interval = useStatisticsStore((s) => s.autoRefreshInterval);
  return useQuery({
    queryKey: ["performance"],
    queryFn: fetchPerformance,
    refetchInterval: interval > 0 ? Math.max(interval, 15_000) : false,
  });
}

export function useModels() {
  const interval = useStatisticsStore((s) => s.autoRefreshInterval);
  return useQuery({
    queryKey: ["models"],
    queryFn: fetchModels,
    refetchInterval: interval > 0 ? Math.max(interval, 15_000) : false,
  });
}

export function useRegimes() {
  const interval = useStatisticsStore((s) => s.autoRefreshInterval);
  return useQuery({
    queryKey: ["regimes"],
    queryFn: fetchRegimes,
    refetchInterval: interval > 0 ? Math.max(interval, 15_000) : false,
  });
}

export function useLatency() {
  const interval = useStatisticsStore((s) => s.autoRefreshInterval);
  return useQuery({
    queryKey: ["latency"],
    queryFn: fetchLatency,
    refetchInterval: interval > 0 ? Math.min(Math.max(interval, 5_000), 15_000) : false,
  });
}

export function useHealth() {
  const interval = useStatisticsStore((s) => s.autoRefreshInterval);
  return useQuery({
    queryKey: ["health"],
    queryFn: fetchHealth,
    refetchInterval: interval > 0 ? Math.min(Math.max(interval, 3_000), 10_000) : false,
  });
}

export function useRounds(filter: RoundsFilter) {
  const interval = useStatisticsStore((s) => s.autoRefreshInterval);
  return useQuery({
    queryKey: ["rounds", filter],
    queryFn: () => fetchRounds(filter),
    refetchInterval: interval > 0 ? Math.max(interval, 15_000) : false,
  });
}

