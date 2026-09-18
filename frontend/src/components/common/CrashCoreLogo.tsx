"use client";

import Image from "next/image";
import { useState } from "react";
import clsx from "clsx";

interface LogoProps {
  size?: "sm" | "md" | "lg";
  showText?: boolean;
  className?: string;
}

export function CrashCoreLogo({ size = "md", showText = true, className }: LogoProps) {
  const [imgError, setImgError] = useState(false);

  const dimMap = {
    sm: { box: "h-7 w-7", img: 28, text: "text-sm", sub: "text-[8px]" },
    md: { box: "h-9 w-9", img: 36, text: "text-base", sub: "text-[9px]" },
    lg: { box: "h-12 w-12", img: 48, text: "text-xl", sub: "text-[10px]" },
  };

  const { box, img, text } = dimMap[size];

  return (
    <div className={clsx("flex items-center gap-2.5 select-none", className)}>
      {/* Emblem Icon / Logo Image */}
      <div
        className={clsx(
          box,
          "relative flex items-center justify-center rounded-lg overflow-hidden bg-black border border-surface-border shadow-md shrink-0"
        )}
      >
        {!imgError ? (
          <Image
            src="/logo.png"
            alt="CrashCore Logo"
            width={img}
            height={img}
            className="object-contain w-full h-full p-0.5"
            onError={() => setImgError(true)}
            priority
          />
        ) : (
          /* High-Fidelity SVG Vector Fallback representing the metallic C and red rocket arrow */
          <svg viewBox="0 0 100 100" className="w-full h-full p-1" fill="none">
            <defs>
              <linearGradient id="silverGrad" x1="0%" y1="0%" x2="100%" y2="100%">
                <stop offset="0%" stopColor="#ffffff" />
                <stop offset="50%" stopColor="#d1d5db" />
                <stop offset="100%" stopColor="#9ca3af" />
              </linearGradient>
              <linearGradient id="redGrad" x1="0%" y1="100%" x2="100%" y2="0%">
                <stop offset="0%" stopColor="#ef4444" />
                <stop offset="50%" stopColor="#dc2626" />
                <stop offset="100%" stopColor="#b91c1c" />
              </linearGradient>
            </defs>
            {/* Metallic C arc */}
            <path
              d="M 68 28 C 60 18 45 16 34 23 C 20 32 18 52 26 66 C 34 80 54 84 68 74 C 73 70 77 64 78 58 L 64 54 C 63 58 60 61 56 64 C 47 69 34 66 29 57 C 24 48 26 36 34 30 C 42 24 53 26 58 33 Z"
              fill="url(#silverGrad)"
            />
            {/* Red ascending speed arrow */}
            <path
              d="M 22 76 L 36 62 L 31 59 L 55 42 L 53 58 L 47 55 L 34 69 Z"
              fill="url(#redGrad)"
            />
            <polygon points="56,36 68,26 66,42" fill="#ef4444" />
          </svg>
        )}
      </div>

      {/* Brand Typography */}
      {showText && (
        <div className="flex flex-col">
          <div className="flex items-center gap-1.5 leading-none">
            <span className={clsx("font-black tracking-[0.16em] text-white font-mono", text)}>
              CRASH<span className="text-red-500">CORE</span>
            </span>
          </div>
          <span className="text-[9px] font-mono tracking-widest uppercase text-ink-muted mt-0.5">
            C++ TELEMETRY OBSERVATORY
          </span>
        </div>
      )}
    </div>
  );
}
