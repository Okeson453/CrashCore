"use client";

import React, { useState } from "react";
import { usePWAInstall } from "@/hooks/usePWAInstall";
import { Download, Smartphone, X, CheckCircle2, Share } from "lucide-react";
import clsx from "clsx";

interface PWAInstallButtonProps {
  variant?: "header" | "compact" | "banner";
  className?: string;
}

export function PWAInstallButton({ variant = "header", className }: PWAInstallButtonProps) {
  const { isInstallable, isInstalled, isIOS, install } = usePWAInstall();
  const [showIOSGuide, setShowIOSGuide] = useState(false);

  // If already installed, hide the button
  if (isInstalled) {
    return null;
  }

  // Desktop / Android flow with active prompt
  if (isInstallable) {
    return (
      <button
        onClick={install}
        className={clsx(
          "group flex items-center gap-1.5 rounded-lg border border-red-500/40 bg-red-950/30 px-2.5 py-1 text-xs font-semibold text-red-300 transition-all hover:bg-red-900/50 hover:border-red-500/70 hover:text-white shadow-sm",
          className
        )}
        title="Install CrashCore to mobile device or desktop"
      >
        <Smartphone className="h-3.5 w-3.5 text-red-400 group-hover:scale-110 transition-transform" />
        <span className="hidden sm:inline">Install App</span>
        <span className="sm:hidden">Install</span>
      </button>
    );
  }

  // iOS Safari flow
  if (isIOS) {
    return (
      <>
        <button
          onClick={() => setShowIOSGuide(true)}
          className={clsx(
            "group flex items-center gap-1.5 rounded-lg border border-surface-border bg-surface-raised/80 px-2.5 py-1 text-xs font-semibold text-ink-muted transition-all hover:bg-surface-subtle hover:text-ink shadow-sm",
            className
          )}
          title="Install CrashCore on iPhone or iPad"
        >
          <Smartphone className="h-3.5 w-3.5 text-ink-subtle group-hover:text-ink transition-colors" />
          <span>Install on iOS</span>
        </button>

        {showIOSGuide && (
          <div className="fixed inset-0 z-50 flex items-center justify-center bg-black/70 backdrop-blur-sm p-4 animate-in fade-in duration-200">
            <div className="w-full max-w-sm rounded-xl border border-surface-border bg-surface-panel p-6 shadow-2xl">
              <div className="flex items-center justify-between pb-3 border-b border-surface-borderSubtle">
                <div className="flex items-center gap-2">
                  <Smartphone className="h-5 w-5 text-accent-cyan" />
                  <h3 className="text-base font-bold text-ink">Install CrashCore PWA</h3>
                </div>
                <button
                  onClick={() => setShowIOSGuide(false)}
                  className="rounded p-1 text-ink-muted hover:bg-surface-raised hover:text-ink"
                >
                  <X className="h-4 w-4" />
                </button>
              </div>

              <div className="mt-4 space-y-3 text-xs text-ink-muted">
                <p>Install the CrashCore telemetry dashboard directly to your iPhone or iPad home screen:</p>
                <div className="rounded-lg border border-surface-borderSubtle bg-surface-raised/60 p-3 space-y-2">
                  <div className="flex items-start gap-2.5">
                    <span className="flex h-5 w-5 shrink-0 items-center justify-center rounded-full bg-accent-cyan/15 text-[11px] font-bold text-accent-cyan">
                      1
                    </span>
                    <p className="pt-0.5">
                      Tap the <Share className="inline h-3.5 w-3.5 text-accent-cyan mx-0.5" /> <strong>Share</strong> button in the Safari toolbar.
                    </p>
                  </div>
                  <div className="flex items-start gap-2.5">
                    <span className="flex h-5 w-5 shrink-0 items-center justify-center rounded-full bg-accent-cyan/15 text-[11px] font-bold text-accent-cyan">
                      2
                    </span>
                    <p className="pt-0.5">
                      Scroll down and tap <strong>Add to Home Screen</strong>.
                    </p>
                  </div>
                  <div className="flex items-start gap-2.5">
                    <span className="flex h-5 w-5 shrink-0 items-center justify-center rounded-full bg-accent-cyan/15 text-[11px] font-bold text-accent-cyan">
                      3
                    </span>
                    <p className="pt-0.5">
                      Tap <strong>Add</strong> in the top-right corner to launch with native standalone performance.
                    </p>
                  </div>
                </div>
              </div>

              <button
                onClick={() => setShowIOSGuide(false)}
                className="mt-5 w-full rounded-lg bg-surface-raised border border-surface-border py-2 text-xs font-semibold text-ink hover:bg-surface-subtle transition-colors"
              >
                Got It
              </button>
            </div>
          </div>
        )}
      </>
    );
  }

  // Fallback generic button for browsers where install prompt can be triggered or guidance given
  return (
    <button
      onClick={() => {
        if (!install()) {
          setShowIOSGuide(true);
        }
      }}
      className={clsx(
        "group flex items-center gap-1.5 rounded-lg border border-surface-border bg-surface-raised/80 px-2.5 py-1 text-xs font-semibold text-ink-muted transition-all hover:bg-surface-subtle hover:text-ink shadow-sm",
        className
      )}
      title="Install CrashCore to your device"
    >
      <Smartphone className="h-3.5 w-3.5 text-ink-dim group-hover:text-ink-muted transition-colors" />
      <span className="hidden sm:inline">Install App</span>
      <span className="sm:hidden">Install</span>
    </button>
  );
}
