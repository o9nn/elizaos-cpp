import { z } from "zod";
import { publicProcedure, router } from "./_core/trpc";

const COGVERSE_HTTP_BASE = (process.env.COGVERSE_HTTP_BASE ?? "http://cogcity.coghood.com").replace(/\/+$/, "");
const ELIZAD_BASE = `${COGVERSE_HTTP_BASE}/api/elizad`;
const EVENT_BASE = `${COGVERSE_HTTP_BASE}/api/events`;
// elizad serializes a large AtomSpace snapshot and currently takes 9–13 seconds
// under live load. Keep a bounded margin while still failing deterministically.
const REQUEST_TIMEOUT_MS = 25_000;

export type CogverseEvent = {
  id: number;
  event_type: string;
  source?: string;
  target?: string | null;
  payload?: Record<string, unknown>;
  timestamp?: number;
  hash?: string;
  tic?: number;
};

export type ResidentDetail = {
  resident: string;
  source: "elizad" | "event_bus";
  episodic_memory: Array<{
    type: string;
    content: string;
    ts: number;
    event_id?: number;
  }>;
  conversation_history: Array<{
    stimulus: string;
    response: string;
    ts: number;
    event_id?: number;
  }>;
  actions: Array<{
    action_type: string;
    result: string;
    ts: number;
    inference_id?: number;
    correlation_id?: string;
    event_id?: number;
  }>;
};

function asRecord(value: unknown): Record<string, unknown> {
  return value && typeof value === "object" && !Array.isArray(value)
    ? (value as Record<string, unknown>)
    : {};
}

function asText(value: unknown): string {
  return typeof value === "string" ? value : "";
}

function asNumber(value: unknown): number | undefined {
  return typeof value === "number" && Number.isFinite(value) ? value : undefined;
}

function eventTimestamp(event: CogverseEvent): number {
  const raw = event.timestamp ?? 0;
  return raw > 10_000_000_000 ? raw : raw * 1_000;
}

function eventResident(event: CogverseEvent): string {
  const payload = asRecord(event.payload);
  return (
    asText(payload.resident) ||
    asText(event.target) ||
    (event.source && event.source !== "dashboard" ? event.source : "")
  ).toLowerCase();
}

export function deriveResidentDetail(
  resident: string,
  events: CogverseEvent[],
): ResidentDetail {
  const name = resident.toLowerCase();
  const episodic: ResidentDetail["episodic_memory"] = [];
  const conversations: ResidentDetail["conversation_history"] = [];
  const actions: ResidentDetail["actions"] = [];
  const pendingStimuli = new Map<string, { message: string; ts: number; event_id: number }>();

  const ordered = [...events].sort((a, b) => (a.id ?? 0) - (b.id ?? 0));
  for (const event of ordered) {
    const payload = asRecord(event.payload);
    const ts = eventTimestamp(event);

    if (event.event_type === "resident.stimulus" && event.target?.toLowerCase() === name) {
      const message = asText(payload.message) || "Directed dashboard stimulus";
      episodic.push({ type: "stimulus", content: message, ts, event_id: event.id });
      const correlationId = asText(payload.correlation_id);
      if (correlationId) pendingStimuli.set(correlationId, { message, ts, event_id: event.id });
      continue;
    }

    if (event.event_type === "resident.thought" && eventResident(event) === name) {
      const thought = asText(payload.thought) || asText(payload.message);
      if (!thought) continue;
      episodic.push({ type: "thought", content: thought, ts, event_id: event.id });
      const correlationId = asText(payload.correlation_id);
      const stimulus = correlationId ? pendingStimuli.get(correlationId) : undefined;
      if (stimulus) {
        conversations.push({
          stimulus: stimulus.message,
          response: thought,
          ts,
          event_id: event.id,
        });
        pendingStimuli.delete(correlationId);
      }
      continue;
    }

    if (event.event_type === "resident.action_result" && eventResident(event) === name) {
      const actionType = asText(payload.action_type) || "action";
      const result = asText(payload.result) || "Action completed";
      actions.push({
        action_type: actionType,
        result,
        ts,
        inference_id: asNumber(payload.inference_id),
        correlation_id: asText(payload.correlation_id) || undefined,
        event_id: event.id,
      });
      episodic.push({
        type: "action",
        content: `${actionType}: ${result}`,
        ts,
        event_id: event.id,
      });
    }
  }

  return {
    resident: name,
    source: "event_bus",
    episodic_memory: episodic.slice(-10).reverse(),
    conversation_history: conversations.slice(-5).reverse(),
    actions: actions.slice(-10).reverse(),
  };
}

async function fetchJson<T>(url: string, init?: RequestInit): Promise<T> {
  const response = await fetch(url, {
    ...init,
    signal: AbortSignal.timeout(REQUEST_TIMEOUT_MS),
    headers: {
      Accept: "application/json",
      ...(init?.headers ?? {}),
    },
  });
  if (!response.ok) {
    throw new Error(`CogCity request failed (${response.status})`);
  }
  return response.json() as Promise<T>;
}

async function recentEvents(since = 0, limit = 1_000) {
  const params = new URLSearchParams({ since_tic: String(since), limit: String(limit) });
  return fetchJson<CogverseEvent[]>(`${EVENT_BASE}/events?${params}`);
}

export const cogverseRouter = router({
  snapshot: publicProcedure.query(async () => {
    const [elizad, atomspace] = await Promise.all([
      fetchJson<Record<string, unknown>>(`${ELIZAD_BASE}/state`),
      fetchJson<Record<string, unknown>>(`${ELIZAD_BASE}/atomspace`),
    ]);
    return { elizad, atomspace, fetchedAt: Date.now() };
  }),

  events: publicProcedure
    .input(
      z.object({
        since: z.number().int().nonnegative().default(0),
        limit: z.number().int().min(1).max(1_000).default(50),
      }),
    )
    .query(({ input }) => recentEvents(input.since, input.limit)),

  stimulus: publicProcedure
    .input(
      z.object({
        target: z.string().min(1).max(64),
        message: z.string().trim().min(1).max(4_000),
        correlationId: z.string().min(1).max(128),
      }),
    )
    .mutation(async ({ input }) => {
      const event = {
        type: "resident.stimulus",
        source: "dashboard",
        target: input.target,
        payload: {
          message: input.message,
          target: input.target,
          correlation_id: input.correlationId,
        },
      };
      const result = await fetchJson<Record<string, unknown>>(`${EVENT_BASE}/publish`, {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify(event),
      });
      return {
        accepted: true as const,
        correlationId: input.correlationId,
        target: input.target,
        result,
        sentAt: Date.now(),
      };
    }),

  resident: publicProcedure
    .input(z.object({ resident: z.string().min(1).max(64) }))
    .query(async ({ input }) => {
      try {
        const upstream = await fetchJson<ResidentDetail>(
          `${ELIZAD_BASE}/resident/${encodeURIComponent(input.resident)}`,
        );
        return { ...upstream, source: "elizad" as const };
      } catch {
        return deriveResidentDetail(input.resident, await recentEvents(0, 1_000));
      }
    }),
});
