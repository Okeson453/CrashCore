"use client";

import {
  ResponsiveContainer,
  BarChart,
  Bar,
  XAxis,
  YAxis,
  Tooltip,
  CartesianGrid,
} from "recharts";

export function WinRateChart({
  data,
}: {
  data: { window: number; rate: number }[];
}) {
  const points = data.map((d) => ({
    window: `${d.window}`,
    rate: Number((d.rate * 100).toFixed(2)),
  }));
  return (
    <div className="h-56 w-full">
      <ResponsiveContainer>
        <BarChart data={points} margin={{ top: 8, right: 8, left: 0, bottom: 0 }}>
          <CartesianGrid stroke="#2a2e3a" strokeDasharray="3 3" />
          <XAxis dataKey="window" tick={{ fill: "#6b7385", fontSize: 11 }} />
          <YAxis tick={{ fill: "#6b7385", fontSize: 11 }} width={40} domain={[0, 100]} />
          <Tooltip
            contentStyle={{ background: "#12141a", border: "1px solid #2a2e3a", fontSize: 12 }}
            formatter={(v: number) => [`${v}%`, "Win rate"]}
          />
          <Bar dataKey="rate" fill="#34d399" radius={[2, 2, 0, 0]} />
        </BarChart>
      </ResponsiveContainer>
    </div>
  );
}
