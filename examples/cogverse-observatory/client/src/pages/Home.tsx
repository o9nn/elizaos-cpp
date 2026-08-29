import Starfield from "@/components/Starfield";
import GearIndicator from "@/components/GearIndicator";
import ThoughtStream from "@/components/ThoughtStream";
import MetricsPanel from "@/components/MetricsPanel";
import StimulusInput from "@/components/StimulusInput";
import ResidentInspector from "@/components/ResidentInspector";
import { useVillageState } from "@/hooks/useVillageState";
import { useState } from "react";

const RESIDENTS = ["echo", "marduk", "aion", "manus", "opencog", "vega", "ember", "ma9us"];

export default function Home() {
  const [selectedResident, setSelectedResident] = useState<string | null>(null);
  const [targetResident, setTargetResident] = useState("echo");
  const { elizad, atomspace, thoughts, connected, transport, lastUpdate, stimulus, sendStimulus, clearStimulus } =
    useVillageState();

  const afResidents = (atomspace?.attentional_focus || []).map((r: string) => r.toLowerCase());
  const gearStates = atomspace?.gear_states || [];

  return (
    <div className="min-h-screen relative overflow-hidden">
      <Starfield />

      {/* Content layer */}
      <div className="relative z-10">
        {/* Header */}
        <header className="py-6 px-4 flex items-center justify-between">
          <div className="flex items-center gap-3">
            {/* Gear logo */}
            <svg width="32" height="32" viewBox="0 0 32 32">
              <circle cx="16" cy="16" r="12" fill="none" stroke="#00f0ff" strokeWidth="1.5" strokeDasharray="3 2" />
              <circle cx="16" cy="16" r="4" fill="#00f0ff" opacity="0.6" />
              <circle cx="16" cy="16" r="2" fill="#00f0ff" />
            </svg>
            <h1 className="font-pixel text-sm text-[#00f0ff] glow-cyan tracking-wider">
              CogVerse
            </h1>
          </div>
          <div className="flex items-center gap-3">
            <span
              className={`w-2 h-2 rounded-full ${connected ? "bg-green-400 animate-pulse" : "bg-red-500"}`}
            />
            <span className="font-mono text-[10px] text-[#ffffff50]">
              {connected
                ? `LIVE · tic ${elizad?.village_tic?.toLocaleString() || "?"} · ${new Date(lastUpdate).toLocaleTimeString()}`
                : "CONNECTING..."}
            </span>
          </div>
        </header>

        {/* Hero section */}
        <section className="px-4 mb-8">
          <div className="relative max-w-5xl mx-auto">
            {/* Glow behind hero */}
            <div className="absolute inset-0 blur-3xl opacity-20 bg-gradient-to-r from-[#00f0ff] via-transparent to-[#ffb347]" />
            <img
              src="/cogverse_hero.png"
              alt="CogVerse Village — CogHood origin server connected to CogCity compute cluster by a glowing bridge, nine pixel residents crossing between floating islands in dark space"
              className="w-full h-auto relative rounded-sm"
            />
            {/* Overlay text */}
            <div className="absolute bottom-4 left-4 right-4 flex justify-between items-end">
              <div>
                <h2 className="font-pixel text-[10px] sm:text-xs text-[#ffb347] glow-amber mb-1">
                  Nine minds. Five gear trains. One village.
                </h2>
                <p className="font-mono text-[10px] text-[#ffffff60]">
                  {elizad
                    ? `Mode: ${elizad.cognitive_mode} · Valence: ${elizad.valence.toFixed(2)} · ${elizad.group_dynamics.residents_online} online`
                    : "The bridge hums both ways."}
                </p>
              </div>
              <div className="font-pixel text-[8px] text-[#00f0ff60]">
                {atomspace ? `${atomspace.stats.atom_count.toLocaleString()} atoms` : "..."}
              </div>
            </div>
          </div>
        </section>

        {/* Main dashboard grid */}
        <section className="px-4 max-w-6xl mx-auto">
          {/* Gear trains row */}
          <div className="mb-6">
            <h3 className="font-pixel text-[9px] text-[#ffffff40] uppercase tracking-widest mb-3 pl-1">
              Antikythera Gear Trains
            </h3>
            <div className="grid grid-cols-2 md:grid-cols-4 gap-3">
              {gearStates.length > 0
                ? gearStates.map((gear) => (
                    <GearIndicator
                      key={gear.train}
                      gear={gear}
                      afResidents={afResidents}
                      selectedResident={selectedResident}
                      onResidentSelect={setSelectedResident}
                      highlightedResident={stimulus?.target || targetResident}
                      stimulusStatus={stimulus?.status || "idle"}
                    />
                  ))
                : Array.from({ length: 4 }).map((_, i) => (
                    <div key={i} className="border-dashed-cyan panel-bg p-4 h-32 flex items-center justify-center">
                      <span className="font-mono text-xs text-[#ffffff20]">...</span>
                    </div>
                  ))}
            </div>
          </div>

          {/* Two-column: Metrics + Thoughts */}
          <div className="grid md:grid-cols-2 gap-4 mb-6">
            <MetricsPanel atomspace={atomspace} elizad={elizad} connected={connected} />
            <ThoughtStream thoughts={thoughts} transport={transport} />
          </div>

          {/* Stimulus input */}
          <div className="mb-8">
            <StimulusInput
              onSend={sendStimulus}
              residents={RESIDENTS}
              tracker={stimulus}
              onTargetChange={setTargetResident}
              onClearTracker={clearStimulus}
            />
          </div>
        </section>

        {/* Footer */}
        <footer className="py-8 px-4 text-center border-t border-[#00f0ff10]">
          <p className="font-pixel text-[8px] text-[#ffffff30] tracking-wider">
            CogVerse Observatory · CogHood ↔ CogCity · The village exists because we are building it together
          </p>
        </footer>
      </div>
      <ResidentInspector
        resident={selectedResident}
        open={Boolean(selectedResident)}
        onOpenChange={open => !open && setSelectedResident(null)}
      />
    </div>
  );
}
