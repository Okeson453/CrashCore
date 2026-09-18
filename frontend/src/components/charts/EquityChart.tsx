"use client";

import {
  ResponsiveContainer,
  LineChart,
  Line,
  XAxis,
  YAxis,
  Tooltip,
  CartesianGrid,
} from "recharts";
import type { PerformancePoint } from "@/types/statistics";

export function EquityChart({ data }: { data: PerformancePoint[] }) {
  const points = data.map((d) => ({
    t: new Date(d.t).toLocaleDateString(undefined, { month: "short", day: "numeric" }),
    equity: d.equity,
  }));
  return (
    <div className="h-64 w-full">
      <ResponsiveContainer>
        <LineChart data={points} margin={{ top: 8, right: 8, left: 0, bottom: 0 }}>
          <CartesianGrid stroke="#2a2e3a" strokeDasharray="3 3" />
          <XAxis dataKey="t" tick={{ fill: "#6b7385", fontSize: 11 }} />
          <YAxis tick={{ fill: "#6b7385", fontSize: 11 }} width={48} />
          <Tooltip
            contentStyle={{ background: "#12141a", border: "1px solid #2a2e3a", fontSize: 12 }}
            labelStyle={{ color: "#9aa3b5" }}
          />
          <Line type="monotone" dataKey="equity" stroke="#22d3ee" strokeWidth={1.5} dot={false} />
        </LineChart>
      </ResponsiveContainer>
    </div>
  );
}
