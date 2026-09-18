import type { Config } from "tailwindcss";

const config: Config = {
  content: ["./src/**/*.{js,ts,jsx,tsx,mdx}"],
  theme: {
    extend: {
      colors: {
        surface: {
          DEFAULT: "#0a0b0d",
          panel: "#12141a",
          raised: "#1a1d26",
          border: "#2a2e3a",
        },
        ink: {
          DEFAULT: "#e8eaef",
          muted: "#9aa3b5",
          dim: "#6b7385",
        },
        accent: {
          live: "#22d3ee",
          positive: "#34d399",
          negative: "#f87171",
          warning: "#fbbf24",
        },
      },
      fontFamily: {
        mono: ["ui-monospace", "SFMono-Regular", "Menlo", "Monaco", "Consolas", "monospace"],
        sans: ["Inter", "system-ui", "sans-serif"],
      },
    },
  },
  plugins: [],
};
export default config;
