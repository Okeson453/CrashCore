"use client";

import Link from "next/link";
import { usePathname, useRouter } from "next/navigation";
import clsx from "clsx";
import { ConnectionBadge } from "@/components/system-status/ConnectionBadge";
import { useStatisticsStore } from "@/stores/statistics-store";
import { useEffect, useState } from "react";
import { connectStatisticsStream } from "@/data/websocket/statistics-stream";
import { useQueryClient } from "@tanstack/react-query";
import {
  LayoutDashboard,
  TrendingUp,
  LineChart,
  Cpu,
  Layers,
  Zap,
  Database,
  Keyboard,
  X,
  Radio,
  Server,
  Terminal,
} from "lucide-react";

const PRIMARY_NAV = [
  { href: "/", label: "Overview", icon: LayoutDashboard, key: "1" },
  { href: "/predictions", label: "Predictions", icon: TrendingUp, key: "2" },
  { href: "/performance", label: "Performance", icon: LineChart, key: "3" },
  { href: "/models", label: "Models", icon: Cpu, key: "4" },
  { href: "/latency", label: "Latency", icon: Zap, key: "5" },
];

const TOP_RIGHT_NAV = [
  { href: "/regimes", label: "Regimes", icon: Layers, key: "6" },
  { href: "/data", label: "Data Explorer", icon: Database, key: "7" },
];

const ALL_NAV = [...PRIMARY_NAV, ...TOP_RIGHT_NAV];

export function AppShell({ children }: { children: React.ReactNode }) {
  const pathname = usePathname();
  const router = useRouter();
  const queryClient = useQueryClient();

  const connection = useStatisticsStore((s) => s.connection);
  const lastUpdate = useStatisticsStore((s) => s.lastUpdate);
  const sequence = useStatisticsStore((s) => s.sequence);
  const dataAgeMs = useStatisticsStore((s) => s.dataAgeMs);
  const soundEnabled = useStatisticsStore((s) => s.soundEnabled);
  const setConnection = useStatisticsStore((s) => s.setConnection);
  const applyUpdate = useStatisticsStore((s) => s.applyUpdate);
  const tickAge = useStatisticsStore((s) => s.tickAge);

  const [showShortcuts, setShowShortcuts] = useState(false);

  // Connect WebSocket/Stream & tick data age
  useEffect(() => {
    const disconnect = connectStatisticsStream({
      onConnection: setConnection,
      onUpdate: (u) => {
        applyUpdate(u);
        if (soundEnabled) {
          try {
            const ctx = new (window.AudioContext || (window as unknown as { webkitAudioContext: typeof AudioContext }).webkitAudioContext)();
            const osc = ctx.createOscillator();
            const gain = ctx.createGain();
            osc.type = "sine";
            osc.frequency.setValueAtTime(880, ctx.currentTime);
            gain.gain.setValueAtTime(0.015, ctx.currentTime);
            gain.gain.exponentialRampToValueAtTime(0.0001, ctx.currentTime + 0.08);
            osc.connect(gain);
            gain.connect(ctx.destination);
            osc.start();
            osc.stop(ctx.currentTime + 0.08);
          } catch {
            // AudioContext not available in some headless contexts
          }
        }
      },
    });
    const age = setInterval(tickAge, 500);
    return () => {
      disconnect();
      clearInterval(age);
    };
  }, [setConnection, applyUpdate, tickAge, soundEnabled]);

  // Global Keyboard Shortcuts
  useEffect(() => {
    const handleKeyDown = (e: KeyboardEvent) => {
      // Don't trigger shortcuts if user is typing in an input
      if (
        document.activeElement?.tagName === "INPUT" ||
        document.activeElement?.tagName === "SELECT" ||
        document.activeElement?.tagName === "TEXTAREA"
      ) {
        return;
      }

      if (e.key === "?") {
        e.preventDefault();
        setShowShortcuts((prev) => !prev);
      } else if (e.key.toLowerCase() === "r") {
        e.preventDefault();
        queryClient.invalidateQueries();
      } else if (e.key >= "1" && e.key <= "7") {
        const index = parseInt(e.key, 10) - 1;
        if (ALL_NAV[index]) {
          e.preventDefault();
          router.push(ALL_NAV[index].href);
        }
      }
    };

    window.addEventListener("keydown", handleKeyDown);
    return () => window.removeEventListener("keydown", handleKeyDown);
  }, [router, queryClient]);

  return (
    <div className="min-h-screen bg-surface text-ink flex flex-col selection:bg-accent-cyan/20 selection:text-accent-cyan">
      {/* Top Telemetry Header */}
      <header className="sticky top-0 z-40 border-b border-surface-border bg-surface-panel/95 backdrop-blur-md">
        <div className="mx-auto flex max-w-[1440px] flex-col gap-2.5 px-4 pt-3 pb-2">
          {/* Top Bar: Brand on Left, Regimes + Data Explorer + Connection on Right */}
          <div className="flex flex-wrap items-center justify-between gap-3">
            {/* Logo & Brand */}
            <div className="flex items-center gap-3">
              <div className="flex h-8 w-8 items-center justify-center rounded-md bg-accent-cyan/10 border border-accent-cyan/30 text-accent-cyan shadow-sm">
                <Radio className="h-4 w-4 animate-pulse" />
              </div>
              <div>
                <div className="flex items-center gap-2">
                  <span className="text-base font-bold tracking-[0.18em] text-ink font-mono">
                    CRASH<span className="text-accent-cyan">CORE</span>
                  </span>
                  <span className="rounded border border-surface-border bg-surface-raised px-1.5 py-0.5 text-[9px] font-mono uppercase tracking-widest text-ink-dim">
                    C++ ENGINE OBSERVATORY
                  </span>
                  <span className="hidden sm:inline-flex rounded border border-emerald-950/50 bg-accent-emerald/10 px-1.5 py-0.5 text-[9px] font-mono uppercase tracking-wider text-accent-emerald">
                    ACIE ACTIVE
                  </span>
                </div>
                <div className="text-[10px] text-ink-dim font-mono tracking-tight hidden md:block">
                  TELEMETRY & ANALYTICS INTERFACE • ZERO-COPY INGESTION
                </div>
              </div>
            </div>

            {/* Right Top Side: Regimes & Data Explorer menus + Connection Badge */}
            <div className="flex items-center flex-wrap gap-2.5">
              {/* Elevated Top-Right Menus */}
              <div className="flex items-center gap-1.5 rounded-lg border border-surface-border bg-surface-raised/80 p-1 shadow-sm">
                {TOP_RIGHT_NAV.map((item) => {
                  const active = pathname.startsWith(item.href);
                  const Icon = item.icon;
                  return (
                    <Link
                      key={item.href}
                      href={item.href}
                      className={clsx(
                        "group relative flex items-center gap-1.5 rounded-md px-2.5 py-1 text-xs font-semibold transition-all duration-150 whitespace-nowrap",
                        active
                          ? "bg-accent-cyan/15 text-accent-cyan border border-accent-cyan/40 shadow-sm"
                          : "text-ink-muted hover:bg-surface-subtle hover:text-ink border border-transparent"
                      )}
                    >
                      <Icon
                        className={clsx(
                          "h-3.5 w-3.5 transition-colors",
                          active ? "text-accent-cyan" : "text-ink-subtle group-hover:text-ink-muted"
                        )}
                      />
                      <span>{item.label}</span>
                      <span
                        className={clsx(
                          "hidden sm:inline-block rounded px-1 font-mono text-[9px]",
                          active
                            ? "bg-accent-cyan/20 text-accent-cyan"
                            : "text-ink-dim opacity-40 group-hover:opacity-100"
                        )}
                      >
                        {item.key}
                      </span>
                    </Link>
                  );
                })}
              </div>

              {/* Connection & Live Controls */}
              <ConnectionBadge
                state={connection}
                lastUpdate={lastUpdate}
                sequence={sequence}
                dataAgeMs={dataAgeMs}
              />
            </div>
          </div>

          {/* Lower Navigation Bar: Primary Telemetry Pipeline */}
          <div className="flex items-center justify-between border-t border-surface-borderSubtle pt-2">
            <nav className="flex items-center gap-1 overflow-x-auto no-scrollbar py-0.5">
              {PRIMARY_NAV.map((item) => {
                const active =
                  item.href === "/" ? pathname === "/" : pathname.startsWith(item.href);
                const Icon = item.icon;
                return (
                  <Link
                    key={item.href}
                    href={item.href}
                    className={clsx(
                      "group relative flex items-center gap-2 rounded-md px-3 py-1.5 text-xs font-medium transition-all duration-150 whitespace-nowrap",
                      active
                        ? "bg-surface-subtle text-ink shadow-sm border border-surface-border"
                        : "text-ink-muted hover:bg-surface-raised hover:text-ink"
                    )}
                  >
                    <Icon
                      className={clsx(
                        "h-3.5 w-3.5 transition-colors",
                        active ? "text-accent-cyan" : "text-ink-subtle group-hover:text-ink-muted"
                      )}
                    />
                    <span>{item.label}</span>
                    <span
                      className={clsx(
                        "hidden lg:inline-block rounded px-1 font-mono text-[9px]",
                        active
                          ? "bg-surface-panel text-accent-cyan border border-accent-cyan/30"
                          : "text-ink-dim opacity-40 group-hover:opacity-100"
                      )}
                    >
                      {item.key}
                    </span>
                    {active && (
                      <span className="absolute bottom-0 left-2 right-2 h-[2px] rounded-full bg-accent-cyan" />
                    )}
                  </Link>
                );
              })}
            </nav>

            {/* Keyboard Shortcuts Trigger */}
            <button
              onClick={() => setShowShortcuts(true)}
              title="Keyboard Shortcuts (?)"
              className="hidden md:flex items-center gap-1.5 rounded border border-surface-border bg-surface-panel px-2 py-1 text-[11px] font-mono text-ink-dim transition-colors hover:border-surface-subtle hover:text-ink"
            >
              <Keyboard className="h-3 w-3" />
              <span>[ ? ]</span>
            </button>
          </div>
        </div>
      </header>

      {/* Main Content Stage */}
      <main className="mx-auto w-full max-w-[1440px] flex-1 px-4 py-6">{children}</main>

      {/* Telemetry Footer */}
      <footer className="mt-auto border-t border-surface-border bg-surface-panel/60 py-4 text-xs text-ink-dim font-mono">
        <div className="mx-auto flex max-w-[1440px] flex-wrap items-center justify-between gap-4 px-4">
          <div className="flex items-center gap-3">
            <div className="flex items-center gap-1.5">
              <Server className="h-3.5 w-3.5 text-accent-cyan" />
              <span>CrashCore C++ Ingestion:</span>
              <span className="text-accent-emerald">READY</span>
            </div>
            <span className="text-surface-border">•</span>
            <div className="flex items-center gap-1.5">
              <Terminal className="h-3.5 w-3.5 text-ink-subtle" />
              <span>API Gateway:</span>
              <span className="text-ink-muted">/api/statistics/*</span>
            </div>
          </div>

          <div className="flex items-center gap-4 text-[11px]">
            <span>Target: &lt;50ms latency budget</span>
            <span className="text-surface-border">•</span>
            <span>Read-Only Observatory</span>
          </div>
        </div>
      </footer>

      {/* Keyboard Shortcuts Dialog Modal */}
      {showShortcuts && (
        <div className="fixed inset-0 z-50 flex items-center justify-center bg-black/70 backdrop-blur-sm p-4">
          <div className="relative w-full max-w-md rounded-lg border border-surface-border bg-surface-panel p-5 shadow-2xl">
            <div className="flex items-center justify-between border-b border-surface-border pb-3">
              <div className="flex items-center gap-2">
                <Keyboard className="h-4 w-4 text-accent-cyan" />
                <h3 className="text-sm font-semibold text-ink">Keyboard Shortcuts</h3>
              </div>
              <button
                onClick={() => setShowShortcuts(false)}
                className="rounded p-1 text-ink-dim hover:bg-surface-raised hover:text-ink"
              >
                <X className="h-4 w-4" />
              </button>
            </div>

            <div className="mt-4 space-y-2.5 text-xs font-mono">
              <div className="text-[11px] uppercase tracking-wider text-ink-dim font-sans font-semibold">
                Navigation
              </div>
              <div className="grid grid-cols-2 gap-2">
                {ALL_NAV.map((n) => (
                  <div
                    key={n.key}
                    className="flex items-center justify-between rounded border border-surface-borderSubtle bg-surface-raised/40 px-2.5 py-1.5"
                  >
                    <span className="text-ink-muted">{n.label}</span>
                    <kbd className="rounded border border-surface-border bg-surface-subtle px-1.5 py-0.5 text-[10px] text-accent-cyan font-bold">
                      {n.key}
                    </kbd>
                  </div>
                ))}
              </div>

              <div className="pt-2 text-[11px] uppercase tracking-wider text-ink-dim font-sans font-semibold">
                Actions
              </div>
              <div className="space-y-1.5">
                <div className="flex items-center justify-between rounded border border-surface-borderSubtle bg-surface-raised/40 px-2.5 py-1.5">
                  <span className="text-ink-muted">Force Query Invalidation & Refresh</span>
                  <kbd className="rounded border border-surface-border bg-surface-subtle px-1.5 py-0.5 text-[10px] text-accent-cyan font-bold">
                    R
                  </kbd>
                </div>
                <div className="flex items-center justify-between rounded border border-surface-borderSubtle bg-surface-raised/40 px-2.5 py-1.5">
                  <span className="text-ink-muted">Toggle Shortcuts Modal</span>
                  <kbd className="rounded border border-surface-border bg-surface-subtle px-1.5 py-0.5 text-[10px] text-accent-cyan font-bold">
                    ?
                  </kbd>
                </div>
              </div>
            </div>

            <div className="mt-5 flex justify-end">
              <button
                onClick={() => setShowShortcuts(false)}
                className="rounded border border-surface-border bg-surface-raised px-3 py-1.5 text-xs font-medium text-ink transition-colors hover:bg-surface-subtle"
              >
                Close
              </button>
            </div>
          </div>
        </div>
      )}
    </div>
  );
}
