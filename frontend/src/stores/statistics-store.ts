"use client";

import { create } from "zustand";
import type { ConnectionState, OverviewStats, StatisticsUpdate } from "@/types/statistics";

type StatisticsStore = {
  connection: ConnectionState;
  lastUpdate: string | null;
  sequence: number;
  dataAgeMs: number;
  overviewPatch: Partial<OverviewStats> | null;
  lastFlashTimestamp: number;
  autoRefreshInterval: number; // in ms, 0 = paused
  soundEnabled: boolean;
  isManualRefreshing: boolean;
  setConnection: (s: ConnectionState) => void;
  applyUpdate: (u: StatisticsUpdate) => void;
  tickAge: () => void;
  setAutoRefreshInterval: (interval: number) => void;
  toggleSound: () => void;
  setIsManualRefreshing: (b: boolean) => void;
};

export const useStatisticsStore = create<StatisticsStore>((set, get) => ({
  connection: "DISCONNECTED",
  lastUpdate: null,
  sequence: 0,
  dataAgeMs: 0,
  overviewPatch: null,
  lastFlashTimestamp: 0,
  autoRefreshInterval: 15000,
  soundEnabled: false,
  isManualRefreshing: false,

  setConnection: (connection) => set({ connection }),

  applyUpdate: (u) =>
    set({
      lastUpdate: u.timestamp,
      sequence: u.sequence,
      dataAgeMs: 0,
      lastFlashTimestamp: Date.now(),
      overviewPatch: {
        totalRounds: u.rounds,
        totalPredictions: u.predictions,
        wins: u.wins,
        losses: u.losses,
        winRate: u.winRate,
        sequence: u.sequence,
        updatedAt: u.timestamp,
      },
    }),

  tickAge: () => {
    const last = get().lastUpdate;
    if (!last) return;
    set({ dataAgeMs: Date.now() - new Date(last).getTime() });
  },

  setAutoRefreshInterval: (autoRefreshInterval) => set({ autoRefreshInterval }),
  toggleSound: () => set((s) => ({ soundEnabled: !s.soundEnabled })),
  setIsManualRefreshing: (isManualRefreshing) => set({ isManualRefreshing }),
}));
