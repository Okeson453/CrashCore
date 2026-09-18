"use client";

import { KpiCard } from "@/components/kpi/KpiCard";
import { LatencyTable } from "@/components/tables/LatencyTable";
import { useLatency } from "@/data/queries/use-statistics";
import { fmtNum, fmtMs } from "@/lib/formatting/numbers";
import {
  ResponsiveContainer,
  BarChart,
  Bar,
  XAxis,
  YAxis,
  Tooltip,
  CartesianGrid,
  Legend,
} from "recharts";

export default function LatencyPage() {
  const { data, isLoading } = useLatency();

  if (isLoading && !data) return <p className="text-sm text-ink-muted">Loading latency…</p>;
  if (!data) return <p className="text-sm text-ink-muted">No latency statistics.</p>;

  const chart = data.stages.map((s) => ({
    stage: s.stage.replace(" ", "\n"),
    p50: s.p50,
    p95: s.p95,
    p99: s.p99,
  }));

  return (
    <div className="space-y-5">
      <div>
        <h1 className="text-lg font-medium">Latency Dashboard</h1>
        <p className="text-xs text-ink-dim">Critical-path stage timings · events & queue health</p>
      </div>

      <div className="grid grid-cols-2 gap-2 sm:grid-cols-3 lg:grid-cols-6">
        <KpiCard label="Events/sec" value={fmtNum(data.eventsPerSec, 2)} tone="live" />
        <KpiCard label="Predictions/sec" value={fmtNum(data.predictionsPerSec, 2)} />
        <KpiCard label="Dropped" value={fmtNum(data.droppedEvents)} tone="warning" />
        <KpiCard label="Duplicates" value={fmtNum(data.duplicateEvents)} />
        <KpiCard label="Backlog" value={fmtNum(data.processingBacklog)} />
        <KpiCard label="Queue depth" value={fmtNum(data.queueDepth)} />
      </div>

      <section className="rounded border border-surface-border bg-surface-panel p-3">
        <h2 className="mb-2 text-sm font-medium text-ink-muted">Stage latency (ms)</h2>
        <div className="h-72 w-full">
          <ResponsiveContainer>
            <BarChart data={chart} margin={{ top: 8, right: 8, left: 0, bottom: 40 }}>
              <CartesianGrid stroke="#2a2e3a" strokeDasharray="3 3" />
              <XAxis dataKey="stage" tick={{ fill: "#6b7385", fontSize: 10 }} interval={0} angle={-20} textAnchor="end" />
              <YAxis tick={{ fill: "#6b7385", fontSize: 11 }} width={40} />
              <Tooltip
                contentStyle={{ background: "#12141a", border: "1px solid #2a2e3a", fontSize: 12 }}
                formatter={(v: number) => fmtMs(v)}
              />
              <Legend />
              <Bar dataKey="p50" fill="#22d3ee" radius={[2, 2, 0, 0]} />
              <Bar dataKey="p95" fill="#fbbf24" radius={[2, 2, 0, 0]} />
              <Bar dataKey="p99" fill="#f87171" radius={[2, 2, 0, 0]} />
            </BarChart>
          </ResponsiveContainer>
        </div>
      </section>

      <LatencyTable stages={data.stages} />
    </div>
  );
}
