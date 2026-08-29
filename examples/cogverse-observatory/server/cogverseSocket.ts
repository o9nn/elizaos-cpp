import type { Server } from "http";
import type { IncomingMessage } from "http";
import type { Duplex } from "stream";
import WebSocket, { WebSocketServer } from "ws";

const UPSTREAM_WS = process.env.COGVERSE_WS_URL ?? "ws://cogcity.coghood.com/ws/events";

export function websocketFrameText(data: WebSocket.RawData) {
  if (typeof data === "string") return data;
  if (data instanceof ArrayBuffer) return Buffer.from(data).toString("utf8");
  if (Array.isArray(data)) return Buffer.concat(data).toString("utf8");
  return data.toString("utf8");
}

function statusPayload(status: string, detail?: string) {
  return JSON.stringify({
    type: "bridge.status",
    status,
    detail,
    timestamp: Date.now(),
  });
}

export function registerCogverseSocket(server: Server) {
  const bridge = new WebSocketServer({ noServer: true });

  server.on("upgrade", (request: IncomingMessage, socket: Duplex, head: Buffer) => {
    const url = new URL(request.url ?? "/", `http://${request.headers.host ?? "localhost"}`);
    if (url.pathname !== "/api/cogverse/ws") return;

    bridge.handleUpgrade(request, socket, head, client => {
      bridge.emit("connection", client, request);
    });
  });

  bridge.on("connection", client => {
    const upstream = new WebSocket(UPSTREAM_WS, {
      handshakeTimeout: 8_000,
    });

    client.send(statusPayload("connecting"));

    upstream.on("open", () => {
      if (client.readyState === WebSocket.OPEN) {
        client.send(statusPayload("connected"));
      }
    });

    upstream.on("message", data => {
      // `ws` surfaces upstream text frames as Buffer by default. Forwarding the
      // Buffer unchanged turns it into a binary browser frame (Blob), which the
      // dashboard cannot JSON.parse. Preserve the upstream JSON as UTF-8 text.
      if (client.readyState === WebSocket.OPEN) client.send(websocketFrameText(data));
    });

    upstream.on("error", error => {
      if (client.readyState === WebSocket.OPEN) {
        client.send(statusPayload("error", error.message));
      }
    });

    upstream.on("close", () => {
      if (client.readyState === WebSocket.OPEN) {
        client.send(statusPayload("disconnected"));
        client.close(1012, "CogCity upstream disconnected");
      }
    });

    client.on("close", () => {
      if (upstream.readyState === WebSocket.OPEN || upstream.readyState === WebSocket.CONNECTING) {
        upstream.close();
      }
    });
  });
}
