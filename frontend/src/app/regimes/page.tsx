"use client";

import { RegimesTable } from "@/components/tables/RegimesTable";
import { useRegimes } from "@/data/queries/use-statistics";
import {
  ResponsiveContainer,
  BarChart,
  Bar,
  XAxis,
  YAxis,
  Tooltip,
  CartesianGrid,
} from "recharts";
import { fmtPct } from "@/lib/formatting/numbers";

export default function RegimesPage() {
  const { data, isLoading } = useRegimes();

  if (isLoading && !data) return <p className="text-sm text-ink-muted">Loading regime statistics…</p>;
  if (!data) return <p className="text-sm text-ink-muted">No regime statistics.</p>;

  const chart = data.map((r) => ({
    regime: r.regime,
    winRate: Number((r.winRate * 100).toFixed(2)),
  }));

  return (
    <div className="space-y-5">
      <div>
        <h1 className="text-lg font-medium">Regime Statistics</h1>
        <p className="text-xs text-ink-dim">Observed outcomes stratified by market regime</p>
      </div>

      <section className="rounded border border-surface-border bg-surface-panel p-3">
        <h2 className="mb-2 text-sm font-medium text-ink-muted">Win rate by regime</h2>
        <div className="h-56 w-full">
          <ResponsiveContainer>
            <BarChart data={chart} margin={{ top: 8, right: 8, left: 0, bottom: 0 }}>
              <CartesianGrid stroke="#2a2e3a" strokeDasharray="3 3" />
              <XAxis dataKey="regime" tick={{ fill: "#6b7385", fontSize: 11 }} />
              <YAxis tick={{ fill: "#6b7385", fontSize: 11 }} width={40} domain={[0, 100]} />
              <Tooltip
                contentStyle={{ background: "#12141a", border: "1px solid #2a2e3a", fontSize: 12 }}
                formatter={(v: number) => [`${v}%`, "Win rate"]}
              />
              <Bar dataKey="winRate" fill="#22d3ee" radius={[2, 2, 0, 0]} />
            </BarChart>
          </ResponsiveContainer>
        </div>
      </section>

      <RegimesTable rows={data} />
    </div>
  );
}
