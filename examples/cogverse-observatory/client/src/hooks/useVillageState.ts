import { trpc } from "@/lib/trpc";
import { useCallback, useEffect, useMemo, useRef, useState } from "react";

export interface GearState {
  train: string;
  rpm: number;
  modulation: number;
  members: string[];
  antikythera_coupling?: { enabled: boolean; blend_factor: number };
}

export interface AtomSpaceState {
  stats: {
    atom_count: number;
    cycle_count: number;
    resident_count: number;
    total_sti: number;
    event_count: number;
    af_size: number;
  };
  attentional_focus: string[];
  gear_states: GearState[];
}

export interface ElizadState {
  status: string;
  resident: string;
  cognitive_cycles: number;
  cognitive_mode: string;
  events_received: number;
  events_published: number;
  bus_connected: boolean;
  uptime_seconds: number;
  village_tic: number;
  valence: number;
  arousal: number;
  completed_goals: number;
  open_goals: number;
  total_actions: number;
  last_plan: string;
  antikythera: {
    rpm_eliza: number;
    phase_eliza: number;
    epicyclic_mod: number;
  };
  group_dynamics: {
    active_groups: number;
    residents_online: number;
  };
}

export interface BusEvent {
  id: number;
  event_type: string;
  source?: string;
  target?: string | null;
  payload?: Record<string, unknown>;
  timestamp?: number;
  hash?: string;
  tic?: number;
}

export type EventTransport = "connecting" | "websocket" | "fallback" | "offline";
export type StimulusStatus = "idle" | "sending" | "waiting" | "responded" | "timeout" | "error";

export interface StimulusTracker {
  correlationId: string;
  target: string;
  message: string;
  status: StimulusStatus;
  sentAt: number;
  response?: string;
  responseEventId?: number;
  error?: string;
}

export interface VillageState {
  elizad: ElizadState | null;
  atomspace: AtomSpaceState | null;
  thoughts: BusEvent[];
  connected: boolean;
  transport: EventTransport;
  lastUpdate: number;
  error: string | null;
  stimulus: StimulusTracker | null;
}

function eventResident(event: BusEvent) {
  const payloadResident = event.payload?.resident;
  if (typeof payloadResident === "string") return payloadResident.toLowerCase();
  if (event.source && event.source !== "dashboard") return event.source.toLowerCase();
  return "";
}

function eventTimestampMs(event: BusEvent) {
  const raw = event.timestamp ?? 0;
  return raw > 10_000_000_000 ? raw : raw * 1_000;
}

export function matchesStimulusResponse(event: BusEvent, tracker: StimulusTracker) {
  if (event.event_type !== "resident.thought") return false;
  if (eventResident(event) !== tracker.target.toLowerCase()) return false;
  return event.payload?.correlation_id === tracker.correlationId;
}

function isInteresting(event: BusEvent) {
  return event.event_type !== "heartbeat" && event.event_type !== "eliza.heartbeat";
}

export function useVillageState() {
  const [thoughts, setThoughts] = useState<BusEvent[]>([]);
  const [transport, setTransport] = useState<EventTransport>("connecting");
  const [stimulus, setStimulus] = useState<StimulusTracker | null>(null);
  const [lastUpdate, setLastUpdate] = useState(0);
  const seenIds = useRef(new Set<number>());
  const reconnectTimer = useRef<number | null>(null);
  const reconnectAttempt = useRef(0);
  const socketRef = useRef<WebSocket | null>(null);

  const snapshot = trpc.cogverse.snapshot.useQuery(undefined, {
    refetchInterval: 15_000,
    retry: 2,
  });
  const fallbackInput = useMemo(() => ({ since: 0, limit: 100 }), []);
  const fallback = trpc.cogverse.events.useQuery(fallbackInput, {
    enabled: transport !== "websocket",
    refetchInterval: transport === "offline" ? 4_000 : 8_000,
    retry: 1,
  });
  const stimulusMutation = trpc.cogverse.stimulus.useMutation();

  const ingestEvent = useCallback((event: BusEvent) => {
    if (!event || typeof event.id !== "number") return;
    if (seenIds.current.has(event.id)) return;
    seenIds.current.add(event.id);
    if (seenIds.current.size > 2_000) {
      const newest = Array.from(seenIds.current).sort((a, b) => b - a).slice(0, 1_000);
      seenIds.current = new Set(newest);
    }

    setLastUpdate(Date.now());
    if (isInteresting(event)) {
      setThoughts(previous => [event, ...previous].slice(0, 100));
    }

    setStimulus(current => {
      if (!current || current.status !== "waiting") return current;
      if (!matchesStimulusResponse(event, current)) return current;
      const response = event.payload?.thought;
      if (typeof response !== "string" || !response.trim()) return current;
      return {
        ...current,
        status: "responded",
        response,
        responseEventId: event.id,
      };
    });
  }, []);

  useEffect(() => {
    fallback.data?.forEach(event => ingestEvent(event as BusEvent));
    if (fallback.data?.length && transport !== "websocket") setTransport("fallback");
  }, [fallback.data, ingestEvent, transport]);

  useEffect(() => {
    let disposed = false;

    const connect = () => {
      if (disposed) return;
      setTransport("connecting");
      const protocol = window.location.protocol === "https:" ? "wss:" : "ws:";
      const socket = new WebSocket(`${protocol}//${window.location.host}/api/cogverse/ws`);
      socketRef.current = socket;

      socket.onopen = () => {
        reconnectAttempt.current = 0;
      };
      socket.onmessage = message => {
        try {
          const parsed = JSON.parse(String(message.data)) as BusEvent & {
            type?: string;
            status?: string;
          };
          if (parsed.type === "bridge.status") {
            if (parsed.status === "connected") setTransport("websocket");
            if (parsed.status === "error" || parsed.status === "disconnected") {
              setTransport("fallback");
            }
            return;
          }
          if (parsed.type === "welcome") return;
          setTransport("websocket");
          ingestEvent(parsed);
        } catch {
          // Ignore malformed frames; the next valid event keeps the stream alive.
        }
      };
      socket.onerror = () => setTransport("fallback");
      socket.onclose = () => {
        if (disposed) return;
        setTransport("fallback");
        const delay = Math.min(30_000, 1_000 * 2 ** reconnectAttempt.current++);
        reconnectTimer.current = window.setTimeout(connect, delay);
      };
    };

    connect();
    return () => {
      disposed = true;
      if (reconnectTimer.current) window.clearTimeout(reconnectTimer.current);
      socketRef.current?.close();
    };
  }, [ingestEvent]);

  useEffect(() => {
    if (!stimulus || stimulus.status !== "waiting") return;
    const remaining = Math.max(0, 45_000 - (Date.now() - stimulus.sentAt));
    const timeout = window.setTimeout(() => {
      setStimulus(current =>
        current?.status === "waiting" ? { ...current, status: "timeout" } : current,
      );
    }, remaining);
    return () => window.clearTimeout(timeout);
  }, [stimulus]);

  const sendStimulus = useCallback(
    async (target: string, message: string) => {
      const correlationId = crypto.randomUUID();
      const sentAt = Date.now();
      setStimulus({ correlationId, target, message, status: "sending", sentAt });
      try {
        await stimulusMutation.mutateAsync({ target, message, correlationId });
        setStimulus(current =>
          current?.correlationId === correlationId
            ? { ...current, status: "waiting" }
            : current,
        );
        return correlationId;
      } catch (error) {
        setStimulus(current =>
          current?.correlationId === correlationId
            ? {
                ...current,
                status: "error",
                error: error instanceof Error ? error.message : "Stimulus delivery failed",
              }
            : current,
        );
        throw error;
      }
    },
    [stimulusMutation],
  );

  const clearStimulus = useCallback(() => setStimulus(null), []);
  const elizad = (snapshot.data?.elizad as unknown as ElizadState | undefined) ?? null;
  const atomspace = (snapshot.data?.atomspace as unknown as AtomSpaceState | undefined) ?? null;

  return {
    elizad,
    atomspace,
    thoughts,
    connected: Boolean(elizad) && transport !== "offline",
    transport,
    lastUpdate: lastUpdate || snapshot.data?.fetchedAt || 0,
    error: snapshot.error?.message ?? fallback.error?.message ?? null,
    stimulus,
    sendStimulus,
    clearStimulus,
    refresh: snapshot.refetch,
  } satisfies VillageState & {
    sendStimulus: (target: string, message: string) => Promise<string>;
    clearStimulus: () => void;
    refresh: typeof snapshot.refetch;
  };
}
