"use client";

import { KpiCard } from "@/components/kpi/KpiCard";
import { EquityChart } from "@/components/charts/EquityChart";
import { WinRateChart } from "@/components/charts/WinRateChart";
import { DistributionChart } from "@/components/charts/DistributionChart";
import { usePerformance } from "@/data/queries/use-statistics";
import { fmtNum, fmtPct, fmtAge } from "@/lib/formatting/numbers";

export default function PerformancePage() {
  const { data, isLoading } = usePerformance();

  if (isLoading && !data) return <p className="text-sm text-ink-muted">Loading performance…</p>;
  if (!data) return <p className="text-sm text-ink-muted">No performance data.</p>;

  return (
    <div className="space-y-5">
      <div>
        <h1 className="text-lg font-medium">Performance</h1>
        <p className="text-xs text-ink-dim">Historical equity, drawdown, and outcome distribution</p>
      </div>

      <div className="grid grid-cols-2 gap-2 sm:grid-cols-3 lg:grid-cols-5">
        <KpiCard label="Peak Equity" value={fmtNum(data.peakEquity, 1)} />
        <KpiCard label="Current Equity" value={fmtNum(data.currentEquity, 1)} tone="live" />
        <KpiCard label="Current DD" value={fmtPct(data.currentDrawdown)} tone="warning" />
        <KpiCard label="Max DD" value={fmtPct(data.maxDrawdown)} tone="negative" />
        <KpiCard label="Recovery" value={fmtAge(data.recoveryDurationMs)} />
      </div>

      <div className="grid gap-4 lg:grid-cols-2">
        <section className="rounded border border-surface-border bg-surface-panel p-3">
          <h2 className="mb-2 text-sm font-medium text-ink-muted">Cumulative equity</h2>
          <EquityChart data={data.cumulative} />
        </section>
        <section className="rounded border border-surface-border bg-surface-panel p-3">
          <h2 className="mb-2 text-sm font-medium text-ink-muted">Outcome distribution</h2>
          <DistributionChart wins={data.wins} losses={data.losses} skips={data.skips} />
        </section>
      </div>

      <section className="rounded border border-surface-border bg-surface-panel p-3">
        <h2 className="mb-2 text-sm font-medium text-ink-muted">Rolling win rate</h2>
        <WinRateChart data={data.rollingWinRate} />
      </section>
    </div>
  );
}
