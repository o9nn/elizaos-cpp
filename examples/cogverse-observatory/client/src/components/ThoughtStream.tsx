import type { BusEvent, EventTransport } from "@/hooks/useVillageState";

interface ThoughtStreamProps {
  thoughts: BusEvent[];
  transport: EventTransport;
}

const RESIDENT_COLORS: Record<string, string> = {
  manus: "#00f0ff",
  echo: "#ff69b4",
  marduk: "#ff4444",
  aion: "#a855f7",
  opencog: "#22c55e",
  vega: "#3b82f6",
  ember: "#ffb347",
  ma9us: "#f97316",
  dan: "#facc15",
  eliza: "#8b5cf6",
};

function formatEventContent(event: BusEvent): string {
  const payload = event.payload || {};

  // Cognitive state events
  if (event.event_type === "eliza.cognitive_state") {
    const mode = payload.cognitive_mode as string || "unknown";
    const goals = payload.completed_goals as number || 0;
    return `[${mode}] ${goals} goals completed · valence ${(payload.valence as number || 0).toFixed(2)}`;
  }

  // Thought events
  if (payload.thought) return payload.thought as string;
  if (payload.action) return `⚡ ${payload.action as string}`;
  if (payload.message) return payload.message as string;
  if (payload.content) return payload.content as string;

  // Action results
  if (event.event_type?.includes("action")) {
    return `Action: ${JSON.stringify(payload).slice(0, 80)}`;
  }

  // Fallback: show event type and truncated payload
  return `[${event.event_type}] ${JSON.stringify(payload).slice(0, 60)}`;
}

export default function ThoughtStream({ thoughts, transport }: ThoughtStreamProps) {
  if (thoughts.length === 0) {
    return (
      <div className="border-dashed-cyan panel-bg p-4">
        <div className="mb-3 flex items-center justify-between gap-3"><h3 className="font-pixel text-[10px] text-[#00f0ff] uppercase tracking-wider glow-cyan">Thought Stream</h3><span className="font-pixel text-[6px] uppercase tracking-widest text-[#ffffff35]">{transport}</span></div>
        <p className="font-mono text-xs text-[#ffffff40] italic">
          Awaiting village cognition...
        </p>
      </div>
    );
  }

  return (
    <div className="border-dashed-cyan panel-bg p-4 overflow-hidden">
      <div className="mb-3 flex items-center justify-between gap-3"><h3 className="font-pixel text-[10px] text-[#00f0ff] uppercase tracking-wider glow-cyan">Thought Stream</h3><span className={`font-pixel text-[6px] uppercase tracking-widest ${transport === "websocket" ? "text-green-400" : "text-[#ffb347]"}`}>{transport === "websocket" ? "live socket" : `${transport} mode`}</span></div>
      <div className="space-y-3 max-h-[300px] overflow-y-auto pr-2 scrollbar-thin">
        {thoughts.slice(0, 12).map((thought, i) => {
          const source = thought.source || "unknown";
          const color = RESIDENT_COLORS[source] || "#ffffff60";
          const content = formatEventContent(thought);

          return (
            <div
              key={`${thought.id}-${i}`}
              className="flex gap-2 items-start"
              style={{ animation: `float 3s ease-in-out infinite`, animationDelay: `${i * 0.2}s` }}
            >
              {/* Resident badge */}
              <span
                className="font-pixel text-[8px] uppercase shrink-0 px-1.5 py-0.5 mt-0.5"
                style={{
                  color,
                  border: `1px solid ${color}60`,
                  background: `${color}10`,
                }}
              >
                {source.slice(0, 5)}
              </span>
              {/* Event type indicator */}
              <span className="font-mono text-[8px] text-[#ffffff30] shrink-0 mt-1">
                {thought.event_type?.split(".").pop()?.slice(0, 6) || "?"}
              </span>
              {/* Thought content */}
              <p className="font-mono text-[11px] leading-relaxed text-[#ffffffb0] line-clamp-2">
                {content}
              </p>
            </div>
          );
        })}
      </div>
    </div>
  );
}
