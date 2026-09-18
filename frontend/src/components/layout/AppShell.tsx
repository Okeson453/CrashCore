"use client";

import Link from "next/link";
import { usePathname } from "next/navigation";
import clsx from "clsx";
import { ConnectionBadge } from "@/components/system-status/ConnectionBadge";
import { useStatisticsStore } from "@/stores/statistics-store";
import { useEffect } from "react";
import { connectStatisticsStream } from "@/data/websocket/statistics-stream";

const NAV = [
  { href: "/", label: "Overview" },
  { href: "/predictions", label: "Predictions" },
  { href: "/performance", label: "Performance" },
  { href: "/models", label: "Models" },
  { href: "/regimes", label: "Regimes" },
  { href: "/latency", label: "Latency" },
  { href: "/data", label: "Data" },
];

export function AppShell({ children }: { children: React.ReactNode }) {
  const pathname = usePathname();
  const connection = useStatisticsStore((s) => s.connection);
  const lastUpdate = useStatisticsStore((s) => s.lastUpdate);
  const sequence = useStatisticsStore((s) => s.sequence);
  const dataAgeMs = useStatisticsStore((s) => s.dataAgeMs);
  const setConnection = useStatisticsStore((s) => s.setConnection);
  const applyUpdate = useStatisticsStore((s) => s.applyUpdate);
  const tickAge = useStatisticsStore((s) => s.tickAge);

  useEffect(() => {
    const disconnect = connectStatisticsStream({
      onConnection: setConnection,
      onUpdate: applyUpdate,
    });
    const age = setInterval(tickAge, 500);
    return () => {
      disconnect();
      clearInterval(age);
    };
  }, [setConnection, applyUpdate, tickAge]);

  return (
    <div className="min-h-screen bg-surface text-ink">
      <header className="border-b border-surface-border bg-surface-panel">
        <div className="mx-auto flex max-w-[1400px] items-center justify-between px-4 py-3">
          <div className="flex items-center gap-3">
            <span className="text-sm font-semibold tracking-[0.2em]">CRASHCORE</span>
            <span className="rounded border border-surface-border px-1.5 py-0.5 text-[10px] uppercase text-ink-dim">
              Read Only
            </span>
          </div>
          <ConnectionBadge
            state={connection}
            lastUpdate={lastUpdate}
            sequence={sequence}
            dataAgeMs={dataAgeMs}
          />
        </div>
        <nav className="mx-auto flex max-w-[1400px] gap-1 overflow-x-auto px-4 pb-2">
          {NAV.map((item) => {
            const active = pathname === item.href;
            return (
              <Link
                key={item.href}
                href={item.href}
                className={clsx(
                  "rounded px-3 py-1.5 text-sm transition-colors",
                  active
                    ? "bg-surface-raised text-ink"
                    : "text-ink-muted hover:bg-surface-raised/60 hover:text-ink"
                )}
              >
                {item.label}
              </Link>
            );
          })}
        </nav>
      </header>
      <main className="mx-auto max-w-[1400px] px-4 py-5">{children}</main>
    </div>
  );
}
