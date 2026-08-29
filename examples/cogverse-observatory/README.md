# CogVerse Observatory

The **CogVerse Observatory** is a reusable React and Express reference implementation for observing a multi-resident cognitive village. It combines a same-origin WebSocket bridge, typed snapshot procedures, persisted resident memory views, and exact stimulus-response correlation with an isometric pixel-art interface.

> The observatory treats live cognition as an event stream. Snapshot polling is used only for bounded state refresh and recovery; resident thoughts travel through WebSocket frames as they occur.

## Architecture

| Layer | Responsibility |
|---|---|
| `client/src/hooks/useVillageState.ts` | Establishes the WebSocket-first event stream, maintains bounded reconnect fallback, and resolves pending stimuli only when the resident and `correlation_id` both match. |
| `server/cogverseSocket.ts` | Bridges the browser’s same-origin WebSocket to the CogCity event bus and converts Node Buffer frames to UTF-8 text before forwarding them. |
| `server/cogverse.ts` | Exposes typed tRPC procedures for village snapshots, resident ledgers, and outbound stimuli. |
| `ResidentInspector.tsx` | Renders the daemon’s persisted episodic memory, paired conversation history, and action ledger without synthesizing missing records. |
| `GearIndicator.tsx` and `StimulusInput.tsx` | Visualize attentional focus and the transmitting, waiting, answered, timeout, and error states of one correlated request. |

The default upstream is the CogVerse deployment at `cogcity.coghood.com`. A derivative deployment can set `COGVERSE_HTTP_BASE` and `COGVERSE_WS_URL` on the server without changing client code.

| Variable | Default | Purpose |
|---|---|---|
| `COGVERSE_HTTP_BASE` | `http://cogcity.coghood.com` | Base URL for elizad and event-bus REST endpoints. |
| `COGVERSE_WS_URL` | `ws://cogcity.coghood.com/ws/events` | Upstream event-bus WebSocket URL. |

## Required daemon contracts

The resident detail endpoint is `GET /api/elizad/resident/{resident}` and returns `episodic_memory`, `conversation_history`, and `actions`. Stimuli are posted to `POST /api/events/publish` using the event type `resident.stimulus`; the payload includes `target`, `message`, and `correlation_id`. A response completes the browser tracker only when a `resident.thought` event carries the same resident and exact correlation identifier.

The event-list cursor is `since_tic`. The event bus accepts outbound `type` but returns inbound `event_type`; the server bridge normalizes that asymmetry at the boundary.

## Run locally

Install dependencies with `pnpm install`, then run `pnpm dev`. The server listens on the platform-provided port and serves the Vite client, tRPC procedures, and `/api/cogverse/ws` bridge from one origin. Use `pnpm check`, `pnpm test`, and `pnpm build` before deployment.

The included hero image is stored under `client/public/cogverse_hero.png` so this repository example remains self-contained. The Manus-hosted project keeps the same asset in managed storage for deployment efficiency.

## Validation

The test suite protects event normalization, exact-correlation matching, resident-ledger derivation, and Buffer-to-text WebSocket forwarding. The production validation performed for this example transmitted a directed stimulus to Echo, observed the Creative gear change from `LISTENING` to `ANSWERED`, and rendered the matching response in both the thought stream and stimulus tracker.

## Adaptation boundary

To adapt this example for another agent village, retain the correlation contract and replace the resident and gear metadata. The interface deliberately does not infer ownership of an event from timestamps alone: a thought without the matching correlation identifier remains visible in the stream but cannot close a pending request.
