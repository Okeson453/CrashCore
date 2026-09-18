"use client";

import { ResponsiveContainer, PieChart, Pie, Cell, Tooltip, Legend } from "recharts";

const COLORS = ["#34d399", "#f87171", "#6b7385"];

export function DistributionChart({
  wins,
  losses,
  skips,
}: {
  wins: number;
  losses: number;
  skips: number;
}) {
  const data = [
    { name: "Wins", value: wins },
    { name: "Losses", value: losses },
    { name: "Skips", value: skips },
  ];
  return (
    <div className="h-56 w-full">
      <ResponsiveContainer>
        <PieChart>
          <Pie data={data} dataKey="value" nameKey="name" innerRadius={50} outerRadius={80} paddingAngle={2}>
            {data.map((_, i) => (
              <Cell key={i} fill={COLORS[i % COLORS.length]} />
            ))}
          </Pie>
          <Tooltip
            contentStyle={{ background: "#12141a", border: "1px solid #2a2e3a", fontSize: 12 }}
          />
          <Legend wrapperStyle={{ fontSize: 12, color: "#9aa3b5" }} />
        </PieChart>
      </ResponsiveContainer>
    </div>
  );
}
