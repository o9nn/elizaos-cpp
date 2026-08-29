import type { AtomSpaceState, ElizadState } from "@/hooks/useVillageState";
import { useEffect, useRef, useState } from "react";

interface MetricsPanelProps {
  atomspace: AtomSpaceState | null;
  elizad: ElizadState | null;
  connected: boolean;
}

function AnimatedNumber({ value, label, color = "#00f0ff" }: { value: number; label: string; color?: string }) {
  const [display, setDisplay] = useState(0);
  const prevRef = useRef(0);

  useEffect(() => {
    const start = prevRef.current;
    const end = value;
    const duration = 200;
    const startTime = Date.now();

    const animate = () => {
      const elapsed = Date.now() - startTime;
      const progress = Math.min(elapsed / duration, 1);
      const eased = 1 - Math.pow(1 - progress, 3);
      setDisplay(Math.round(start + (end - start) * eased));
      if (progress < 1) requestAnimationFrame(animate);
    };
    animate();
    prevRef.current = value;
  }, [value]);

  return (
    <div className="flex flex-col items-center gap-1">
      <span className="font-mono text-lg font-bold" style={{ color }}>
        {display.toLocaleString()}
      </span>
      <span className="font-pixel text-[7px] uppercase tracking-wider text-[#ffffff50]">
        {label}
      </span>
    </div>
  );
}

function formatUptime(seconds: number): string {
  const h = Math.floor(seconds / 3600);
  const m = Math.floor((seconds % 3600) / 60);
  return h > 0 ? `${h}h ${m}m` : `${m}m`;
}

export default function MetricsPanel({ atomspace, elizad, connected }: MetricsPanelProps) {
  return (
    <div className="border-dashed-cyan panel-bg p-4">
      <div className="flex items-center gap-2 mb-4">
        <h3 className="font-pixel text-[10px] text-[#00f0ff] uppercase tracking-wider glow-cyan">
          AtomSpace
        </h3>
        <span
          className={`w-2 h-2 rounded-full ${connected ? "bg-green-400 animate-pulse" : "bg-red-500"}`}
        />
        <span className="font-mono text-[9px] text-[#ffffff40]">
          {connected ? "LIVE" : "OFFLINE"}
        </span>
        {elizad && (
          <span className="font-mono text-[9px] text-[#ffffff30] ml-auto">
            ↑ {formatUptime(elizad.uptime_seconds)}
          </span>
        )}
      </div>

      {atomspace ? (
        <div className="grid grid-cols-3 gap-3">
          <AnimatedNumber value={atomspace.stats.atom_count} label="Atoms" />
          <AnimatedNumber value={atomspace.stats.cycle_count} label="Cycles" color="#ffb347" />
          <AnimatedNumber value={atomspace.stats.event_count} label="Events" />
          <AnimatedNumber value={Math.round(atomspace.stats.total_sti)} label="Total STI" color="#a855f7" />
          <AnimatedNumber value={elizad?.completed_goals || 0} label="Goals" color="#22c55e" />
          <AnimatedNumber value={elizad?.total_actions || 0} label="Actions" color="#ff69b4" />
        </div>
      ) : (
        <div className="grid grid-cols-3 gap-3">
          {Array.from({ length: 6 }).map((_, i) => (
            <div key={i} className="flex flex-col items-center gap-1">
              <span className="font-mono text-lg font-bold text-[#ffffff20]">---</span>
              <span className="font-pixel text-[7px] uppercase tracking-wider text-[#ffffff20]">
                loading
              </span>
            </div>
          ))}
        </div>
      )}

      {/* Last plan */}
      {elizad?.last_plan && (
        <div className="mt-3 pt-3 border-t border-[#00f0ff15]">
          <span className="font-pixel text-[7px] text-[#ffffff30] uppercase tracking-wider">Last Plan</span>
          <p className="font-mono text-[10px] text-[#ffffff60] mt-1 line-clamp-2">
            {elizad.last_plan}
          </p>
        </div>
      )}
    </div>
  );
}
