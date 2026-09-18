"use client";

import { create } from "zustand";
import type { ConnectionState, OverviewStats, StatisticsUpdate } from "@/types/statistics";

type StatisticsStore = {
  connection: ConnectionState;
  lastUpdate: string | null;
  sequence: number;
  dataAgeMs: number;
  overviewPatch: Partial<OverviewStats> | null;
  setConnection: (s: ConnectionState) => void;
  applyUpdate: (u: StatisticsUpdate) => void;
  tickAge: () => void;
};

export const useStatisticsStore = create<StatisticsStore>((set, get) => ({
  connection: "DISCONNECTED",
  lastUpdate: null,
  sequence: 0,
  dataAgeMs: 0,
  overviewPatch: null,

  setConnection: (connection) => set({ connection }),

  applyUpdate: (u) =>
    set({
      lastUpdate: u.timestamp,
      sequence: u.sequence,
      dataAgeMs: 0,
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
}));
