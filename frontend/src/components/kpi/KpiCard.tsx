"use client";

import clsx from "clsx";
import type { ReactNode } from "react";
import { TrendingUp, TrendingDown } from "lucide-react";

type Props = {
  label: string;
  value: string;
  hint?: string;
  subtitle?: string;
  subValue?: string;
  icon?: ReactNode;
  tone?: "default" | "positive" | "negative" | "warning" | "live" | "cyan" | "indigo";
  trend?: {
    value: string;
    isPositive?: boolean;
  };
  className?: string;
};

export function KpiCard({
  label,
  value,
  hint,
  subtitle,
  subValue,
  icon,
  tone = "default",
  trend,
  className,
}: Props) {
  const secondaryText = subtitle || subValue || hint;

  return (
    <div
      className={clsx(
        "group relative flex flex-col justify-between rounded-xl border border-surface-border bg-surface-panel p-3.5 transition-all duration-150 hover:border-surface-hover hover:bg-surface-raised/40 shadow-sm",
        className
      )}
    >
      <div className="flex items-center justify-between gap-2">
        <span className="text-[11px] font-medium uppercase tracking-wider text-ink-dim font-mono">
          {label}
        </span>
        {icon && (
          <span className="text-ink-subtle transition-colors group-hover:text-ink-muted">
            {icon}
          </span>
        )}
      </div>

      <div className="mt-2 flex items-baseline justify-between gap-2">
        <div
          className={clsx(
            "font-mono text-xl font-bold tabular-nums tracking-tight",
            tone === "positive" && "text-accent-emerald",
            tone === "negative" && "text-accent-rose",
            tone === "warning" && "text-accent-amber",
            (tone === "live" || tone === "cyan") && "text-accent-cyan",
            tone === "indigo" && "text-accent-indigo",
            tone === "default" && "text-ink"
          )}
        >
          {value}
        </div>

        {trend && (
          <div
            className={clsx(
              "flex items-center gap-1 font-mono text-[10px] font-medium tabular-nums",
              trend.isPositive ? "text-accent-emerald" : "text-accent-rose"
            )}
          >
            {trend.isPositive ? (
              <TrendingUp className="h-3 w-3 shrink-0" />
            ) : (
              <TrendingDown className="h-3 w-3 shrink-0" />
            )}
            <span>{trend.value}</span>
          </div>
        )}
      </div>

      {secondaryText && (
        <div className="mt-1.5 flex items-center text-[11px] text-ink-dim font-mono leading-tight">
          {secondaryText}
        </div>
      )}
    </div>
  );
}

