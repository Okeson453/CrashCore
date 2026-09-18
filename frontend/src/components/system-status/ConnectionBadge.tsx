"use client";

import clsx from "clsx";
import type { ConnectionState } from "@/types/statistics";
import { fmtAge, fmtTime, fmtNum } from "@/lib/formatting/numbers";
import { useStatisticsStore } from "@/stores/statistics-store";
import { useQueryClient } from "@tanstack/react-query";
import { Activity, RefreshCw, Volume2, VolumeX, Clock } from "lucide-react";
import { useState, useEffect } from "react";

type Props = {
  state: ConnectionState;
  lastUpdate?: string | null;
  sequence?: number;
  dataAgeMs?: number;
};

export function ConnectionBadge({ state, lastUpdate, sequence, dataAgeMs }: Props) {
  const queryClient = useQueryClient();
  const autoRefreshInterval = useStatisticsStore((s) => s.autoRefreshInterval);
  const setAutoRefreshInterval = useStatisticsStore((s) => s.setAutoRefreshInterval);
  const soundEnabled = useStatisticsStore((s) => s.soundEnabled);
  const toggleSound = useStatisticsStore((s) => s.toggleSound);

  const [isRefreshing, setIsRefreshing] = useState(false);
  const [mounted, setMounted] = useState(false);

  useEffect(() => {
    setMounted(true);
  }, []);

  const handleManualRefresh = async () => {
    setIsRefreshing(true);
    try {
      await queryClient.invalidateQueries();
    } finally {
      setTimeout(() => setIsRefreshing(false), 500);
    }
  };

  const time = lastUpdate != null ? fmtTime(lastUpdate) : "—";

  const isFresh = dataAgeMs != null && dataAgeMs < 10000;
  const isStale = dataAgeMs != null && dataAgeMs >= 30000;

  return (
    <div className="flex flex-wrap items-center justify-between gap-3 text-xs">
      <div className="flex flex-wrap items-center gap-2 sm:gap-3">
        {/* Status indicator badge */}
        <div
          className={clsx(
            "flex items-center gap-1.5 rounded-md px-2.5 py-1 font-mono text-[11px] font-semibold tracking-wider uppercase border",
            state === "LIVE" && "border-accent-cyan/40 bg-accent-cyan/10 text-accent-cyan",
            state === "DEGRADED" && "border-accent-amber/40 bg-accent-amber/10 text-accent-amber",
            state === "STALE" && "border-accent-amber/40 bg-accent-amber/10 text-accent-amber",
            state === "DISCONNECTED" && "border-accent-rose/40 bg-accent-rose/10 text-accent-rose"
          )}
        >
          <span
            className={clsx(
              "h-2 w-2 rounded-full",
              state === "LIVE" && "bg-accent-cyan animate-pulse live-pulse-dot",
              state === "DEGRADED" && "bg-accent-amber",
              state === "STALE" && "bg-accent-amber animate-pulse",
              state === "DISCONNECTED" && "bg-accent-rose"
            )}
          />
          <span>{state === "LIVE" ? "STREAM LIVE" : state}</span>
        </div>

        {/* Timestamp */}
        <div className="hidden sm:flex items-center gap-1.5 text-ink-dim font-mono text-[11px]">
          <Clock className="h-3.5 w-3.5 text-ink-subtle" />
          <span>LAST:</span>
          <span className="text-ink">{time}</span>
        </div>

        {/* Engine Sequence */}
        {sequence != null && (
          <div className="hidden md:flex items-center gap-1 text-[11px] font-mono text-ink-dim">
            <span>SEQ:</span>
            <span className="text-ink-muted">#{fmtNum(sequence)}</span>
          </div>
        )}

        {/* Data age pill */}
        {dataAgeMs != null && (
          <div
            className={clsx(
              "flex items-center gap-1 rounded px-1.5 py-0.5 font-mono text-[11px] tabular-nums",
              isFresh && "text-accent-emerald bg-accent-emerald/10",
              !isFresh && !isStale && "text-ink-muted bg-surface-subtle",
              isStale && "text-accent-amber bg-accent-amber/10"
            )}
          >
            <span>{fmtAge(dataAgeMs)}</span>
            <span className="text-[10px] text-ink-dim">ago</span>
          </div>
        )}
      </div>

      {/* Controls: auto-refresh rate, manual refresh button, sound toggle */}
      <div className="flex items-center gap-2">
        <label className="hidden sm:flex items-center gap-1 text-[11px] text-ink-dim font-mono">
          <Activity className="h-3 w-3 text-ink-subtle" />
          <span>POLL:</span>
          <select
            value={autoRefreshInterval}
            onChange={(e) => setAutoRefreshInterval(Number(e.target.value))}
            className="rounded border border-surface-border bg-surface-raised px-1.5 py-0.5 text-[11px] text-ink outline-none hover:border-surface-subtle cursor-pointer"
          >
            <option value={5000}>5s (Fast)</option>
            <option value={15000}>15s (Normal)</option>
            <option value={30000}>30s (Slow)</option>
            <option value={0}>Paused</option>
          </select>
        </label>

        {/* Audio feedback toggle */}
        <button
          onClick={toggleSound}
          title={soundEnabled ? "Audio ticks enabled" : "Audio ticks muted"}
          className={clsx(
            "flex h-7 w-7 items-center justify-center rounded border border-surface-border transition-colors hover:border-surface-subtle hover:bg-surface-raised",
            soundEnabled ? "text-accent-cyan" : "text-ink-subtle"
          )}
        >
          {soundEnabled ? <Volume2 className="h-3.5 w-3.5" /> : <VolumeX className="h-3.5 w-3.5" />}
        </button>

        {/* Manual refresh button */}
        <button
          onClick={handleManualRefresh}
          title="Force refresh statistics"
          className="flex items-center gap-1 rounded border border-surface-border bg-surface-panel px-2 py-1 text-[11px] font-medium text-ink-muted transition-colors hover:border-surface-subtle hover:bg-surface-raised hover:text-ink"
        >
          <RefreshCw
            className={clsx("h-3 w-3", isRefreshing && "animate-spin text-accent-cyan")}
          />
          <span className="hidden sm:inline">Refresh</span>
        </button>
      </div>
    </div>
  );
}
