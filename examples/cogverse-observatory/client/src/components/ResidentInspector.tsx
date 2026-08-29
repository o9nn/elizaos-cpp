import { Dialog, DialogContent, DialogDescription, DialogHeader, DialogTitle } from "@/components/ui/dialog";
import { Tabs, TabsContent, TabsList, TabsTrigger } from "@/components/ui/tabs";
import { trpc } from "@/lib/trpc";
import { useMemo } from "react";

interface ResidentInspectorProps {
  resident: string | null;
  open: boolean;
  onOpenChange: (open: boolean) => void;
}

function timeLabel(timestamp: number) {
  if (!timestamp) return "time unknown";
  return new Date(timestamp).toLocaleString();
}

function EmptyLedger({ label }: { label: string }) {
  return (
    <div className="border border-dashed border-[#00f0ff24] px-4 py-8 text-center">
      <p className="font-mono text-xs text-[#ffffff45]">
        No {label} are present in the currently exposed memory window.
      </p>
    </div>
  );
}

export default function ResidentInspector({ resident, open, onOpenChange }: ResidentInspectorProps) {
  const input = useMemo(() => ({ resident: resident || "echo" }), [resident]);
  const detail = trpc.cogverse.resident.useQuery(input, {
    enabled: open && Boolean(resident),
    staleTime: 5_000,
    refetchOnWindowFocus: false,
  });
  const data = detail.data;

  return (
    <Dialog open={open} onOpenChange={onOpenChange}>
      <DialogContent className="max-h-[88vh] max-w-3xl overflow-hidden rounded-sm border border-dashed border-[#00f0ff66] bg-[#050510f5] p-0 text-white shadow-[0_0_50px_#00f0ff18] backdrop-blur-xl">
        <DialogHeader className="border-b border-[#00f0ff1f] px-5 py-4 text-left">
          <div className="flex flex-wrap items-start justify-between gap-3 pr-8">
            <div>
              <DialogTitle className="font-pixel text-xs uppercase tracking-[0.18em] text-[#00f0ff] glow-cyan">
                {resident || "resident"} · memory lens
              </DialogTitle>
              <DialogDescription className="mt-2 font-mono text-[11px] text-[#ffffff55]">
                Episodic traces, paired exchanges, and enacted consequences from the live village.
              </DialogDescription>
            </div>
            {data && (
              <span className="border border-[#ffb34755] bg-[#ffb3470d] px-2 py-1 font-pixel text-[7px] uppercase tracking-widest text-[#ffb347]">
                {data.source === "elizad" ? "elizad memory" : "event-bus reconstruction"}
              </span>
            )}
          </div>
        </DialogHeader>

        <div className="max-h-[calc(88vh-105px)] overflow-y-auto px-5 pb-5">
          {detail.isLoading && (
            <div className="py-16 text-center font-pixel text-[8px] uppercase tracking-widest text-[#00f0ff80] animate-pulse">
              Focusing the memory lens…
            </div>
          )}
          {detail.error && (
            <div className="my-5 border border-dashed border-[#ff5c5c66] bg-[#ff5c5c0d] p-4 font-mono text-xs text-[#ff8a8a]">
              The memory lens could not reach CogCity: {detail.error.message}
            </div>
          )}
          {data && (
            <Tabs defaultValue="episodes" className="mt-4">
              <TabsList className="h-auto w-full justify-start gap-1 rounded-none border-b border-[#ffffff14] bg-transparent p-0">
                <TabsTrigger value="episodes" className="rounded-none border-b border-transparent px-3 py-3 font-pixel text-[7px] uppercase text-[#ffffff50] data-[state=active]:border-[#00f0ff] data-[state=active]:bg-[#00f0ff0d] data-[state=active]:text-[#00f0ff]">
                  Episodes · {data.episodic_memory.length}
                </TabsTrigger>
                <TabsTrigger value="conversations" className="rounded-none border-b border-transparent px-3 py-3 font-pixel text-[7px] uppercase text-[#ffffff50] data-[state=active]:border-[#ff69b4] data-[state=active]:bg-[#ff69b40d] data-[state=active]:text-[#ff69b4]">
                  Conversations · {data.conversation_history.length}
                </TabsTrigger>
                <TabsTrigger value="actions" className="rounded-none border-b border-transparent px-3 py-3 font-pixel text-[7px] uppercase text-[#ffffff50] data-[state=active]:border-[#ffb347] data-[state=active]:bg-[#ffb3470d] data-[state=active]:text-[#ffb347]">
                  Actions · {data.actions.length}
                </TabsTrigger>
              </TabsList>

              <TabsContent value="episodes" className="mt-4 space-y-2">
                {data.episodic_memory.length === 0 ? <EmptyLedger label="episodes" /> : data.episodic_memory.map(entry => (
                  <article key={`${entry.event_id ?? entry.ts}-${entry.type}`} className="border border-[#00f0ff20] bg-[#00f0ff07] p-3">
                    <div className="mb-2 flex items-center justify-between gap-3">
                      <span className="font-pixel text-[7px] uppercase tracking-widest text-[#00f0ff]">{entry.type}</span>
                      <time className="font-mono text-[9px] text-[#ffffff35]">{timeLabel(entry.ts)}</time>
                    </div>
                    <p className="whitespace-pre-wrap font-mono text-xs leading-relaxed text-[#ffffffb5]">{entry.content}</p>
                  </article>
                ))}
              </TabsContent>

              <TabsContent value="conversations" className="mt-4 space-y-3">
                {data.conversation_history.length === 0 ? <EmptyLedger label="paired conversations" /> : data.conversation_history.map(entry => (
                  <article key={`${entry.event_id ?? entry.ts}-${entry.stimulus}`} className="border border-[#ff69b429] bg-[#ff69b407] p-3">
                    <time className="mb-3 block text-right font-mono text-[9px] text-[#ffffff35]">{timeLabel(entry.ts)}</time>
                    <div className="border-l-2 border-[#ffb34788] pl-3">
                      <p className="font-pixel text-[7px] uppercase tracking-widest text-[#ffb347]">Stimulus</p>
                      <p className="mt-1 font-mono text-xs leading-relaxed text-[#ffffff85]">{entry.stimulus}</p>
                    </div>
                    <div className="mt-3 border-l-2 border-[#ff69b488] pl-3">
                      <p className="font-pixel text-[7px] uppercase tracking-widest text-[#ff69b4]">Response</p>
                      <p className="mt-1 whitespace-pre-wrap font-mono text-xs leading-relaxed text-[#ffffffc5]">{entry.response}</p>
                    </div>
                  </article>
                ))}
              </TabsContent>

              <TabsContent value="actions" className="mt-4 space-y-2">
                {data.actions.length === 0 ? <EmptyLedger label="action results" /> : data.actions.map(entry => (
                  <article key={`${entry.event_id ?? entry.ts}-${entry.action_type}`} className="border border-[#ffb34729] bg-[#ffb34707] p-3">
                    <div className="mb-2 flex items-center justify-between gap-3">
                      <span className="font-pixel text-[7px] uppercase tracking-widest text-[#ffb347]">{entry.action_type}</span>
                      <time className="font-mono text-[9px] text-[#ffffff35]">{timeLabel(entry.ts)}</time>
                    </div>
                    <p className="font-mono text-xs leading-relaxed text-[#ffffffb5]">{entry.result}</p>
                    {entry.inference_id !== undefined && <p className="mt-2 font-mono text-[9px] text-[#ffffff35]">inference #{entry.inference_id}</p>}
                  </article>
                ))}
              </TabsContent>
            </Tabs>
          )}
        </div>
      </DialogContent>
    </Dialog>
  );
}
