"use client";

import { KpiCard } from "@/components/kpi/KpiCard";
import { useOverview } from "@/data/queries/use-statistics";
import { useStatisticsStore } from "@/stores/statistics-store";
import { fmtNum, fmtPct } from "@/lib/formatting/numbers";
import { isBackendReachable, getLastApiError } from "@/data/api/statistics";

export default function OverviewPage() {
  const { data, isLoading, isError } = useOverview();
  const patch = useStatisticsStore((s) => s.overviewPatch);

  const o = data
    ? {
        ...data,
        ...(patch ?? {}),
      }
    : null;

  if (isLoading && !o) {
    return <p className="text-sm text-ink-muted">Loading statistics…</p>;
  }

  if (!o) {
    return (
      <p className="text-sm text-accent-negative">
        {isError ? "Unable to load overview (showing no data)." : "No data."}
      </p>
    );
  }

  return (
    <div className="space-y-5">
      <div>
        <h1 className="text-lg font-medium">Statistics Dashboard</h1>
        <p className="text-xs text-ink-dim">
          Engine-owned metrics · presentation only · no prediction or control actions
        </p>
        {!isBackendReachable() ? (
          <p className="mt-2 rounded border border-accent-warning/40 bg-accent-warning/10 px-3 py-2 text-xs text-accent-warning">
            Backend offline or unreachable — showing demo/fallback data if available.
            {getLastApiError() ? ` (${getLastApiError()})` : ""} Start CrashCore stats on :8080
            or set CRASHCORE_API_URL.
          </p>
        ) : (
          <p className="mt-2 text-xs text-accent-positive">Connected to CrashCore Statistics API</p>
        )}
      </div>

      <div className="grid grid-cols-2 gap-2 sm:grid-cols-3 lg:grid-cols-4 xl:grid-cols-6">
        <KpiCard label="Total Rounds" value={fmtNum(o.totalRounds)} />
        <KpiCard label="Predictions" value={fmtNum(o.totalPredictions)} />
        <KpiCard label="Wins" value={fmtNum(o.wins)} tone="positive" />
        <KpiCard label="Losses" value={fmtNum(o.losses)} tone="negative" />
        <KpiCard label="Skipped" value={fmtNum(o.skipped)} />
        <KpiCard label="Win Rate" value={fmtPct(o.winRate)} tone="positive" />
        <KpiCard label="Baseline Rate" value={fmtPct(o.baselineRate)} />
        <KpiCard label="Edge" value={fmtPct(o.edge)} tone="live" />
        <KpiCard label="Avg Probability" value={fmtPct(o.avgProbability)} />
        <KpiCard label="Avg Confidence" value={fmtPct(o.avgConfidence)} />
        <KpiCard label="Max Losing Streak" value={String(o.maxLosingStreak)} tone="warning" />
        <KpiCard label="Current Streak" value={String(o.currentLosingStreak)} />
        <KpiCard label="Cumulative P/L" value={fmtNum(o.cumulativePnL, 1)} tone="positive" />
        <KpiCard label="Max Drawdown" value={fmtPct(o.maxDrawdown)} tone="negative" />
      </div>

      <section className="rounded border border-surface-border bg-surface-panel p-4">
        <h2 className="mb-2 text-sm font-medium text-ink-muted">About this view</h2>
        <p className="text-xs leading-relaxed text-ink-dim">
          CrashCore Frontend is a pure statistics observatory. All values are fetched from the
          Statistics API or live stream. The UI never generates predictions, mutates configuration,
          or issues engine commands.
        </p>
      </section>
    </div>
  );
}
