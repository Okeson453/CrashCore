"use client";

import { useState } from "react";
import { RoundsTable } from "@/components/tables/RoundsTable";
import { KpiCard } from "@/components/kpi/KpiCard";
import { MultiplierTrendChart } from "@/components/charts/MultiplierTrendChart";
import { useRounds } from "@/data/queries/use-statistics";
import type { RoundsFilter } from "@/types/statistics";
import { fmtNum, fmtMult, fmtPct } from "@/lib/formatting/numbers";
import {
  Database,
  Filter,
  ShieldCheck,
  Zap,
  TrendingUp,
  Flame,
  AlertTriangle,
  RotateCcw,
  Sparkles,
} from "lucide-react";
import clsx from "clsx";

export default function DataPage() {
  const [filter, setFilter] = useState<RoundsFilter>({ limit: 50 });
  const [quickFilter, setQuickFilter] = useState<"all" | "moon" | "high" | "bust">("all");
  const { data, isLoading, isFetching } = useRounds(filter);

  const rounds = data?.items ?? [];

  // Computed summary metrics over current dataset
  const totalRounds = data?.total ?? rounds.length;
  const avgMultiplier =
    rounds.length > 0
      ? rounds.reduce((acc, r) => acc + r.multiplier, 0) / rounds.length
      : 2.14;
  const moonRounds = rounds.filter((r) => r.multiplier >= 10.0).length;
  const bustRounds = rounds.filter((r) => r.multiplier < 1.2).length;

  const handleQuickFilter = (type: "all" | "moon" | "high" | "bust") => {
    setQuickFilter(type);
    if (type === "all") {
      setFilter((f) => ({ ...f, regime: undefined, cursor: undefined }));
    }
  };

  const displayedRounds = rounds.filter((r) => {
    if (quickFilter === "moon") return r.multiplier >= 10.0;
    if (quickFilter === "high") return r.multiplier >= 3.0;
    if (quickFilter === "bust") return r.multiplier < 1.2;
    return true;
  });

  return (
    <div className="space-y-6">
      {/* Page Header */}
      <div className="flex flex-wrap items-center justify-between gap-3 border-b border-surface-borderSubtle pb-4">
        <div>
          <div className="flex items-center gap-2">
            <h1 className="text-lg font-bold text-ink font-mono">Historical Game Data & Provable Records</h1>
            <span className="rounded bg-accent-cyan/15 px-2 py-0.5 font-mono text-[10px] font-bold text-accent-cyan border border-accent-cyan/30">
              SHA-256 AUDITED
            </span>
          </div>
          <p className="text-xs text-ink-dim">
            Server-side indexed round history, provably fair hashes, and market regime labels
          </p>
        </div>

        <div className="flex items-center gap-2 font-mono text-xs text-ink-dim">
          <span>Archived Database: <strong className="text-ink">{fmtNum(totalRounds)} Rounds</strong></span>
        </div>
      </div>

      {/* Summary KPI Ribbon */}
      <div className="grid grid-cols-2 gap-3 sm:grid-cols-2 md:grid-cols-4">
        <KpiCard
          label="Total Rounds Indexed"
          value={fmtNum(totalRounds)}
          subtitle="Real-time synchronized"
          tone="cyan"
          icon={<Database className="h-4 w-4" />}
        />
        <KpiCard
          label="Sample Mean Multiplier"
          value={fmtMult(avgMultiplier)}
          subtitle="Observed crash point mean"
          icon={<TrendingUp className="h-4 w-4" />}
        />
        <KpiCard
          label="Moon Multipliers (≥10x)"
          value={`${moonRounds} (${fmtPct(moonRounds / Math.max(1, rounds.length))})`}
          subtitle="High-payout anomalies"
          tone="positive"
          icon={<Flame className="h-4 w-4" />}
        />
        <KpiCard
          label="Instant Bust Rate (<1.20x)"
          value={`${bustRounds} (${fmtPct(bustRounds / Math.max(1, rounds.length))})`}
          subtitle="Low multiplier crashes"
          tone="negative"
          icon={<AlertTriangle className="h-4 w-4" />}
        />
      </div>

      {/* Filter & Search Bar */}
      <div className="rounded-xl border border-surface-border bg-surface-panel p-4 shadow-sm space-y-3">
        <div className="flex flex-wrap items-center justify-between gap-3 border-b border-surface-borderSubtle pb-3">
          <div className="flex items-center gap-2">
            <Filter className="h-4 w-4 text-accent-cyan" />
            <h2 className="text-xs font-semibold text-ink font-mono uppercase tracking-wider">
              Filter Query Parameters
            </h2>
          </div>

          {/* Quick Presets */}
          <div className="flex items-center gap-1.5 font-mono text-xs">
            <span className="text-ink-dim mr-1">Quick:</span>
            <button
              onClick={() => handleQuickFilter("all")}
              className={clsx(
                "rounded px-2.5 py-1 text-xs font-medium border transition-colors",
                quickFilter === "all"
                  ? "bg-accent-cyan/15 text-accent-cyan border-accent-cyan/40"
                  : "bg-surface-raised text-ink-dim border-surface-border hover:text-ink"
              )}
            >
              All Data
            </button>
            <button
              onClick={() => handleQuickFilter("moon")}
              className={clsx(
                "rounded px-2.5 py-1 text-xs font-medium border transition-colors",
                quickFilter === "moon"
                  ? "bg-amber-400/20 text-amber-300 border-amber-400/40"
                  : "bg-surface-raised text-ink-dim border-surface-border hover:text-ink"
              )}
            >
              Moon (≥10x)
            </button>
            <button
              onClick={() => handleQuickFilter("high")}
              className={clsx(
                "rounded px-2.5 py-1 text-xs font-medium border transition-colors",
                quickFilter === "high"
                  ? "bg-accent-emerald/20 text-accent-emerald border-accent-emerald/40"
                  : "bg-surface-raised text-ink-dim border-surface-border hover:text-ink"
              )}
            >
              High (≥3x)
            </button>
            <button
              onClick={() => handleQuickFilter("bust")}
              className={clsx(
                "rounded px-2.5 py-1 text-xs font-medium border transition-colors",
                quickFilter === "bust"
                  ? "bg-accent-rose/20 text-accent-rose border-accent-rose/40"
                  : "bg-surface-raised text-ink-dim border-surface-border hover:text-ink"
              )}
            >
              Busts (&lt;1.2x)
            </button>
          </div>
        </div>

        {/* Detailed Form Controls */}
        <form
          className="grid gap-3 sm:grid-cols-2 lg:grid-cols-4"
          onSubmit={(e) => {
            e.preventDefault();
            const fd = new FormData(e.currentTarget);
            setFilter({
              limit: 50,
              regime: String(fd.get("regime") || "") || undefined,
              from: String(fd.get("from") || "") || undefined,
              to: String(fd.get("to") || "") || undefined,
              cursor: undefined,
            });
          }}
        >
          <div>
            <label className="block text-[11px] font-mono text-ink-dim mb-1">
              Start Timestamp
            </label>
            <input
              name="from"
              type="datetime-local"
              className="w-full rounded-md border border-surface-border bg-surface-raised px-2.5 py-1.5 text-xs text-ink outline-none focus:border-accent-cyan/60"
            />
          </div>

          <div>
            <label className="block text-[11px] font-mono text-ink-dim mb-1">
              End Timestamp
            </label>
            <input
              name="to"
              type="datetime-local"
              className="w-full rounded-md border border-surface-border bg-surface-raised px-2.5 py-1.5 text-xs text-ink outline-none focus:border-accent-cyan/60"
            />
          </div>

          <div>
            <label className="block text-[11px] font-mono text-ink-dim mb-1">
              Market Regime Classification
            </label>
            <select
              name="regime"
              className="w-full rounded-md border border-surface-border bg-surface-raised px-2.5 py-1.5 text-xs text-ink outline-none focus:border-accent-cyan/60 cursor-pointer"
              defaultValue=""
            >
              <option value="">All Market Regimes</option>
              <option value="High-Volatility Mean Reversion">High-Volatility Mean Reversion</option>
              <option value="Trending Momentum">Trending Momentum</option>
              <option value="Low-Volatility Range">Low-Volatility Range</option>
              <option value="Severe Crash Regime">Severe Crash Regime</option>
            </select>
          </div>

          <div className="flex items-end gap-2">
            <button
              type="submit"
              className="flex-1 rounded-md border border-accent-cyan/40 bg-accent-cyan/15 px-3 py-1.5 font-mono text-xs font-semibold text-accent-cyan hover:bg-accent-cyan/25 transition-colors"
            >
              Query Telemetry
            </button>
            <button
              type="button"
              onClick={() => {
                setFilter({ limit: 50 });
                setQuickFilter("all");
              }}
              title="Reset Filters"
              className="flex h-[32px] w-[32px] items-center justify-center rounded-md border border-surface-border bg-surface-raised text-ink-dim hover:text-ink transition-colors"
            >
              <RotateCcw className="h-3.5 w-3.5" />
            </button>
          </div>
        </form>
      </div>

      {/* Visual Multiplier Trend Chart */}
      <MultiplierTrendChart rounds={displayedRounds} height={280} />

      {/* Rounds Table Container */}
      <div className="space-y-2">
        <div className="flex items-center justify-between font-mono text-xs text-ink-dim">
          <span>
            Displaying <strong className="text-ink">{displayedRounds.length}</strong> of{" "}
            <strong className="text-ink">{fmtNum(totalRounds)}</strong> recorded game rounds
          </span>
          {isFetching && (
            <span className="flex items-center gap-1.5 text-accent-cyan">
              <span className="h-1.5 w-1.5 rounded-full bg-accent-cyan animate-ping" />
              Refreshing database cache...
            </span>
          )}
        </div>

        <RoundsTable rows={displayedRounds} defaultLimit={15} />

        {data?.nextCursor && (
          <div className="pt-2 text-center">
            <button
              type="button"
              className="rounded-lg border border-surface-border bg-surface-panel px-4 py-2 font-mono text-xs text-ink hover:bg-surface-raised transition-colors shadow-sm"
              onClick={() => setFilter((f) => ({ ...f, cursor: data.nextCursor ?? undefined }))}
            >
              Load Next Page Batch →
            </button>
          </div>
        )}
      </div>
    </div>
  );
}
