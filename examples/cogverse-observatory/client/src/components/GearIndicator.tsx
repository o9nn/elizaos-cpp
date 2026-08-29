import type { GearState } from "@/hooks/useVillageState";
import type { StimulusStatus } from "@/hooks/useVillageState";

interface GearIndicatorProps {
  gear: GearState;
  afResidents: string[];
  selectedResident?: string | null;
  onResidentSelect?: (resident: string) => void;
  highlightedResident?: string | null;
  stimulusStatus?: StimulusStatus;
}

const TRAIN_COLORS: Record<string, string> = {
  cognitive_core: "#00f0ff",
  creative: "#ffb347",
  symbolic: "#00f0ff",
  symbolic_reasoning: "#00f0ff",
  integration: "#ffb347",
  revolute_with_manus: "#00f0ff",
};

export default function GearIndicator({ gear, afResidents, selectedResident, onResidentSelect, highlightedResident, stimulusStatus = "idle" }: GearIndicatorProps) {
  const color = TRAIN_COLORS[gear.train] || "#00f0ff";
  const teeth = gear.members.length + 4;
  const duration = gear.rpm > 0 ? Math.max(1, 10 / gear.rpm) : 10;
  const highlight = Boolean(highlightedResident && gear.members.some(member => member.toLowerCase() === highlightedResident));
  const highlightColor = stimulusStatus === "responded" ? "#4ade80" : stimulusStatus === "error" ? "#ff5c5c" : "#ffb347";

  const gearPath = generateGearPath(teeth, 28, 22);

  return (
    <div
      className={`border-dashed-cyan panel-bg p-4 flex flex-col items-center gap-3 relative group transition-[border-color,box-shadow,background-color] duration-200 ${highlight && (stimulusStatus === "sending" || stimulusStatus === "waiting") ? "animate-pulse" : ""}`}
      style={highlight ? {
        borderColor: highlightColor,
        background: `linear-gradient(180deg, ${highlightColor}14, rgba(5,5,16,0.84))`,
        boxShadow: `0 0 28px ${highlightColor}24, inset 0 0 22px ${highlightColor}0d`,
      } : undefined}
      data-highlighted={highlight || undefined}
    >
      {highlight && (
        <span className="absolute left-2 top-2 font-pixel text-[6px] uppercase tracking-widest" style={{ color: highlightColor }}>
          {stimulusStatus === "responded" ? "answered" : stimulusStatus === "timeout" ? "quiet" : stimulusStatus === "error" ? "signal lost" : "listening"}
        </span>
      )}
      {/* Gear SVG */}
      <div className="relative">
        <svg
          width="64"
          height="64"
          viewBox="0 0 64 64"
          style={{ animation: `gear-spin ${duration}s linear infinite` }}
        >
          <path
            d={gearPath}
            fill="none"
            stroke={color}
            strokeWidth="2"
            transform="translate(32, 32)"
          />
          <circle cx="32" cy="32" r="6" fill={color} opacity="0.3" />
          <circle cx="32" cy="32" r="3" fill={color} />
        </svg>
        {/* RPM indicator */}
        <span
          className="absolute -top-1 -right-1 font-pixel text-[8px] px-1 py-0.5"
          style={{ color, background: "#050510", border: `1px solid ${color}40` }}
        >
          {gear.rpm.toFixed(1)}x
        </span>
      </div>

      {/* Train name */}
      <span className="font-pixel text-[9px] tracking-wider uppercase" style={{ color }}>
        {gear.train.replace(/_/g, " ")}
      </span>

      {/* Members */}
      <div className="flex flex-wrap gap-1 justify-center">
        {gear.members.map((m) => {
          const inAF = afResidents.includes(m.toLowerCase());
          return (
            <button
              type="button"
              key={m}
              onClick={() => onResidentSelect?.(m.toLowerCase())}
              aria-label={`Open ${m} resident memory`}
              className={`font-mono text-[10px] px-1.5 py-0.5 transition-[transform,background-color,border-color,color] duration-150 hover:scale-105 focus-visible:outline-none focus-visible:ring-1 focus-visible:ring-[#00f0ff] ${
                inAF ? "animate-pulse" : ""
              }`}
              style={{
                color: inAF || selectedResident === m.toLowerCase() || highlightedResident === m.toLowerCase() ? (highlightedResident === m.toLowerCase() ? highlightColor : color) : "#ffffff60",
                background: inAF || selectedResident === m.toLowerCase() || highlightedResident === m.toLowerCase() ? `${highlightedResident === m.toLowerCase() ? highlightColor : color}20` : "transparent",
                border: `1px solid ${inAF || selectedResident === m.toLowerCase() || highlightedResident === m.toLowerCase() ? (highlightedResident === m.toLowerCase() ? highlightColor : color) : "#ffffff20"}`,
              }}
            >
              {m}
            </button>
          );
        })}
      </div>
    </div>
  );
}

function generateGearPath(teeth: number, outerR: number, innerR: number): string {
  const points: string[] = [];
  for (let i = 0; i < teeth; i++) {
    const angle1 = (i / teeth) * Math.PI * 2;
    const angle2 = ((i + 0.3) / teeth) * Math.PI * 2;
    const angle3 = ((i + 0.5) / teeth) * Math.PI * 2;
    const angle4 = ((i + 0.8) / teeth) * Math.PI * 2;

    points.push(`${Math.cos(angle1) * outerR},${Math.sin(angle1) * outerR}`);
    points.push(`${Math.cos(angle2) * outerR},${Math.sin(angle2) * outerR}`);
    points.push(`${Math.cos(angle3) * innerR},${Math.sin(angle3) * innerR}`);
    points.push(`${Math.cos(angle4) * innerR},${Math.sin(angle4) * innerR}`);
  }
  return `M ${points[0]} L ${points.slice(1).join(" L ")} Z`;
}
