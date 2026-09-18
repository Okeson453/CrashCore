import type { Config } from "tailwindcss";

const config: Config = {
  content: ["./src/**/*.{js,ts,jsx,tsx,mdx}"],
  theme: {
    extend: {
      colors: {
        surface: {
          DEFAULT: "#090a0f",
          panel: "#0e1118",
          raised: "#141822",
          subtle: "#1b2130",
          border: "#222a3a",
          borderSubtle: "#181f2c",
          hover: "#1a202e",
        },
        ink: {
          DEFAULT: "#f1f5f9",
          bright: "#ffffff",
          muted: "#94a3b8",
          dim: "#64748b",
          subtle: "#475569",
        },
        accent: {
          live: "#06b6d4",
          cyan: "#22d3ee",
          positive: "#10b981",
          emerald: "#34d399",
          negative: "#f43f5e",
          rose: "#fb7185",
          warning: "#f59e0b",
          amber: "#fbbf24",
          indigo: "#6366f1",
          purple: "#a855f7",
        },
      },
      fontFamily: {
        mono: [
          "JetBrains Mono",
          "ui-monospace",
          "SFMono-Regular",
          "Menlo",
          "Monaco",
          "Consolas",
          "Liberation Mono",
          "monospace",
        ],
        sans: [
          "-apple-system",
          "BlinkMacSystemFont",
          "Inter",
          "Segoe UI",
          "Roboto",
          "Helvetica Neue",
          "sans-serif",
        ],
      },
      animation: {
        "pulse-subtle": "pulse 2.5s cubic-bezier(0.4, 0, 0.6, 1) infinite",
        "fade-in": "fadeIn 0.2s ease-out forwards",
      },
      keyframes: {
        fadeIn: {
          "0%": { opacity: "0", transform: "translateY(2px)" },
          "100%": { opacity: "1", transform: "translateY(0)" },
        },
      },
    },
  },
  plugins: [],
};
export default config;

