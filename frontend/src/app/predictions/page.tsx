"use client";

import { KpiCard } from "@/components/kpi/KpiCard";
import { PredictionsTable } from "@/components/tables/PredictionsTable";
import { usePredictions } from "@/data/queries/use-statistics";
import { fmtNum, fmtPct } from "@/lib/formatting/numbers";

export default function PredictionsPage() {
  const { data, isLoading } = usePredictions();

  if (isLoading && !data) return <p className="text-sm text-ink-muted">Loading predictions…</p>;
  if (!data) return <p className="text-sm text-ink-muted">No prediction statistics.</p>;

  return (
    <div className="space-y-5">
      <div>
        <h1 className="text-lg font-medium">Prediction Statistics</h1>
        <p className="text-xs text-ink-dim">Observed outcomes only — no decision logic in the UI</p>
      </div>

      <div className="grid grid-cols-2 gap-2 sm:grid-cols-3 lg:grid-cols-4 xl:grid-cols-6">
        <KpiCard label="Total" value={fmtNum(data.total)} />
        <KpiCard label="Valid" value={fmtNum(data.valid)} />
        <KpiCard label="Skipped" value={fmtNum(data.skipped)} />
        <KpiCard label="Wins" value={fmtNum(data.wins)} tone="positive" />
        <KpiCard label="Losses" value={fmtNum(data.losses)} tone="negative" />
        <KpiCard label="Win Rate" value={fmtPct(data.winRate)} tone="positive" />
        <KpiCard label="Precision" value={fmtPct(data.precision)} />
        <KpiCard label="Recall" value={fmtPct(data.recall)} />
        <KpiCard label="Avg Probability" value={fmtPct(data.avgProbability)} />
        <KpiCard label="Avg Confidence" value={fmtPct(data.avgConfidence)} />
        <KpiCard label="Avg Edge" value={fmtPct(data.avgEdge)} tone="live" />
        <KpiCard label="Calibration Err" value={fmtPct(data.calibrationError)} tone="warning" />
      </div>

      <section>
        <h2 className="mb-2 text-sm font-medium text-ink-muted">Recent predictions</h2>
        <PredictionsTable rows={data.rows} />
      </section>
    </div>
  );
}
