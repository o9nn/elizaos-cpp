import type { StimulusTracker } from "@/hooks/useVillageState";
import { useEffect, useState } from "react";

interface StimulusInputProps {
  onSend: (target: string, message: string) => Promise<string>;
  residents: string[];
  tracker: StimulusTracker | null;
  onTargetChange?: (target: string) => void;
  onClearTracker?: () => void;
}

const RESIDENT_COLORS: Record<string, string> = {
  manus: "#00f0ff",
  echo: "#ffb347",
  marduk: "#ffb347",
  aion: "#00f0ff",
  opencog: "#00f0ff",
  vega: "#00f0ff",
  ember: "#ffb347",
  ma9us: "#00f0ff",
  dan: "#ffb347",
};

export default function StimulusInput({ onSend, residents, tracker, onTargetChange, onClearTracker }: StimulusInputProps) {
  const [target, setTarget] = useState(residents[0] || "echo");
  const [message, setMessage] = useState("");
  const [sending, setSending] = useState(false);

  useEffect(() => onTargetChange?.(target), [onTargetChange, target]);

  const handleSend = async () => {
    if (!message.trim()) return;
    setSending(true);
    try {
      await onSend(target, message);
      setMessage("");
    } finally {
      setSending(false);
    }
  };

  const color = RESIDENT_COLORS[target] || "#00f0ff";

  return (
    <div className="border-dashed-cyan panel-bg p-4">
      <h3 className="font-pixel text-[10px] text-[#ffb347] uppercase tracking-wider mb-3 glow-amber">
        Signal into the village
      </h3>

      {/* Target selector */}
      <div className="flex flex-wrap gap-1 mb-3">
        {residents.map((r) => (
          <button
            key={r}
            onClick={() => setTarget(r)}
            className="font-pixel text-[7px] uppercase px-2 py-1 transition-all duration-150"
            style={{
              color: target === r ? RESIDENT_COLORS[r] || "#fff" : "#ffffff40",
              border: `1px solid ${target === r ? (RESIDENT_COLORS[r] || "#fff") + "80" : "#ffffff20"}`,
              background: target === r ? (RESIDENT_COLORS[r] || "#fff") + "15" : "transparent",
              transform: target === r ? "scale(1.05)" : "scale(1)",
            }}
          >
            {r}
          </button>
        ))}
      </div>

      {/* Message input */}
      <div className="flex gap-2">
        <input
          type="text"
          value={message}
          onChange={(e) => setMessage(e.target.value)}
          onKeyDown={(e) => e.key === "Enter" && handleSend()}
          placeholder={`Address ${target}...`}
          className="flex-1 bg-transparent border font-mono text-xs px-3 py-2 text-[#ffffffd0] placeholder:text-[#ffffff30] focus:outline-none focus:border-opacity-60"
          style={{ borderColor: `${color}40` }}
        />
        <button
          onClick={handleSend}
          disabled={sending || !message.trim()}
          className="font-pixel text-[8px] uppercase px-4 py-2 transition-all duration-150 active:scale-95 disabled:opacity-30"
          style={{
            color: "#050510",
            background: color,
            border: `1px solid ${color}`,
          }}
        >
          {sending ? "..." : "Transmit"}
        </button>
      </div>

      {tracker && (
        <div
          className="mt-3 border border-dashed p-3 transition-[border-color,background-color] duration-200"
          style={{
            borderColor: tracker.status === "responded" ? "#4ade8066" : tracker.status === "error" ? "#ff5c5c66" : `${RESIDENT_COLORS[tracker.target] || "#00f0ff"}55`,
            background: tracker.status === "responded" ? "#4ade800a" : tracker.status === "error" ? "#ff5c5c0a" : `${RESIDENT_COLORS[tracker.target] || "#00f0ff"}08`,
          }}
        >
          <div className="flex items-center justify-between gap-3">
            <div className="flex items-center gap-2">
              <span className={`h-1.5 w-1.5 rounded-full ${tracker.status === "responded" ? "bg-green-400" : tracker.status === "error" ? "bg-red-400" : tracker.status === "timeout" ? "bg-amber-400" : "bg-cyan-300 animate-pulse"}`} />
              <span className="font-pixel text-[7px] uppercase tracking-widest" style={{ color: RESIDENT_COLORS[tracker.target] || "#00f0ff" }}>
                {tracker.target} · {tracker.status === "sending" ? "transmitting" : tracker.status === "waiting" ? "inference active" : tracker.status}
              </span>
            </div>
            {(tracker.status === "responded" || tracker.status === "timeout" || tracker.status === "error") && (
              <button type="button" onClick={onClearTracker} className="font-pixel text-[6px] uppercase text-[#ffffff45] hover:text-white focus-visible:outline-none focus-visible:ring-1 focus-visible:ring-[#00f0ff]">
                clear
              </button>
            )}
          </div>
          <p className="mt-2 font-mono text-[10px] text-[#ffffff65]">“{tracker.message}”</p>
          {tracker.status === "waiting" && <p className="mt-2 font-mono text-[10px] text-[#00f0ff85]">Holding the bridge open for {tracker.target}’s exactly correlated thought…</p>}
          {tracker.status === "timeout" && <p className="mt-2 font-mono text-[10px] text-[#ffb347]">No response arrived inside the 45-second observation window. The live stream remains open.</p>}
          {tracker.error && <p className="mt-2 font-mono text-[10px] text-[#ff8a8a]">{tracker.error}</p>}
          {tracker.response && (
            <div className="mt-3 border-l-2 border-[#4ade80] pl-3">
              <p className="font-pixel text-[6px] uppercase tracking-widest text-[#4ade80]">Inference response · event #{tracker.responseEventId}</p>
              <p className="mt-1 whitespace-pre-wrap font-mono text-xs leading-relaxed text-[#ffffffc5]">{tracker.response}</p>
            </div>
          )}
        </div>
      )}
    </div>
  );
}
