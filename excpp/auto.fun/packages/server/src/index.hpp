#include "allowedOrigins.hpp"
#include "auth/auth.hpp"
#include "cron.hpp"
#include "env.hpp"
#include "mcap.hpp"
#include "middleware/rateLimiter.hpp"
#include "redis.hpp"
#include "routes/admin.hpp"
#include "routes/admin/pregenerated.hpp"
#include "routes/agents.hpp"
#include "routes/auth.hpp"
#include "routes/chat.hpp"
#include "routes/files.hpp"
#include "routes/generation.hpp"
#include "routes/migration.hpp"
#include "routes/share.hpp"
#include "routes/swap.hpp"
#include "routes/token.hpp"
#include "routes/user.hpp"
#include "routes/webhooks.hpp"
#include "util.hpp"
#include "websocket-manager.hpp"
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

;
dotenv.config();

;
;
;
;
import type { WSContext } from "hono/ws"; // Import WSContext type for handlers

;
;
; // Import the cron task runner
; // Assuming Env type is defined and includes Redis vars
;
;
;
;
;
;
;
; // Import the new router
;
;
;
;
;
;
;
;
;
;
;
;

struct AppVariables {
    std::optional<{ publicKey: string } | null> user;
};


const app = new Hono<{ Variables: AppVariables }>();

const env = process.env as unknown as Env;

// Setup Solana connection
const RPC_URL = (
  process.env.NETWORK === "devnet"
    ? process.env.DEVNET_SOLANA_RPC_URL
    : process.env.MAINNET_SOLANA_RPC_URL
)!;

if (!RPC_URL) {
  throw new Error(
    "RPC_URL is not defined. Set NETWORK and corresponding RPC URL in .env"
  );
}
const connection = new Connection(RPC_URL, "confirmed");
logger.info(`Connected to Solana RPC: ${RPC_URL}`);

// --- Middleware ---

// CORS Middleware (from original index.ts)
app.use(
  "*",
  cors({
    origin: allowedOrigins, // Make sure allowedOrigins is compatible
    credentials: true,
    allowMethods: ["GET", "POST", "PUT", "DELETE", "OPTIONS"],
    allowHeaders: [
      "Content-Type",
      "Authorization",
      "X-API-Key",
      "X-Twitter-OAuth-Token",
      "X-Twitter-OAuth-Token-Secret",
      "ngrok-skip-browser-warning", // Added for ngrok testing if needed
    ],
    exposeHeaders: ["Content-Length"],
    maxAge: 60000,
  })
);
const api = new Hono<{ Variables: AppVariables }>();

(async () => {
  const redisCache = await getGlobalRedisCache();
  const limiter = createRateLimiter(redisCache);
  app.use("*", limiter);

  api.use("*", verifyAuth);
  api.route("/generation", generationRouter);
  api.route("/", tokenRouter);

  api.route("/", fileRouter);
  api.route("/", authRouter);
  api.route("/", agentRouter);
  api.route("/", swapRouter);
  api.route("/", chatRouter);
  api.route("/share", shareRouter);
  api.route("/", webhookRouter);
  api.route("/", migrationRouter);
  api.route("/users", userRouter);
  api.route("/admin", adminRouter);
  api.route("/owner", ownerRouter);
  api.route("/admin/pregenerated", preGeneratedAdminRoutes); // Mount the new router

  api.get("/sol-price", async (c) => {
    try {
      const solPrice = await getSOLPrice(); // Use the global cache service
      logger.info("(Placeholder) Would fetch SOL price");
      return c.json({ price: solPrice });
    } catch (error) {
      logger.error("Error fetching SOL price:", error);
      return c.json({ error: "Failed to fetch SOL price" }, 500);
    }
  });

  // --- Mount the API sub-router ---
  app.route("/api", api);
})();

// --- Special Cron Trigger Route ---
// Use a non-standard path and require a secret header
const CRON_SECRET = process.env.CRON_SECRET || "develop"; // Get secret from environment

if (!CRON_SECRET) {
  logger.warn(
    "CRON_SECRET environment variable not set. Cron trigger endpoint will be disabled."
  );
}

// Mount this route directly on the main app, outside /api if desired
app.post("/trigger-cron", async (c) => {
  console.log("Triggering cron");
  const providedSecret = c.req.header("X-Cron-Secret");
  if (providedSecret !== CRON_SECRET) {
    logger.warn("Unauthorized attempt to trigger cron endpoint.");
    return c.json({ error: "Unauthorized" }, 403);
  }

  logger.log(
    "Cron trigger endpoint called successfully. Initiating tasks asynchronously..."
  );

  // Run tasks asynchronously (fire and forget). Do NOT await here.
  // The lock mechanism inside runCronTasks will prevent overlaps.
  await runCronTasks();

  // Return immediately to the cron runner
  return c.json({ success: true, message: "Cron tasks finished." });
});

// --- Root and Maintenance Routes ---
app.get("/", (c) => c.json({ status: "ok", message: "Hono server running!" }));

// --- Not Found Handler ---
app.notFound((c) => {
  logger.warn(`Not Found: ${c.req.method} ${c.req.url}`);
  return c.json(
    {
      error: "Not Found",
      message: `Route ${c.req.method} ${c.req.url} not found.`,
    },
    404
  );
});

// --- Error Handler ---
app.onError((err, c) => {
  logger.error(
    `Unhandled error on ${c.req.path}:`,
    err instanceof Error ? err.stack : err
  );
  // Avoid leaking stack traces in production
  return c.json({ error: "Internal Server Error" }, 500);
});

(async () => {
  // --- Initialize Services ---
  const redisCache = await getGlobalRedisCache();
  logger.info("Redis Cache Service Retrieved.");
  const isReady = await redisCache.isPoolReady();

  console.log("isReady", isReady);

  if (!redisCache) throw new Error("Redis Cache Service not found");

  // Initialize WebSocketManager with Redis
  if (!webSocketManager.redisCache) {
    await webSocketManager.initialize(redisCache);
  }

  if (!webSocketManager.redisCache) {
    throw new Error("WebSocket Manager not initialized");
  }
})().catch((err) => {
  logger.error("Error during initialization:", err);
});

// --- Create Bun WebSocket handlers ---
const { upgradeWebSocket, websocket } = createBunWebSocket();
// --- Add WebSocket Upgrade Route ---
app.get(
  "/ws",
  upgradeWebSocket((c: Context) => {
    return {
      onOpen: (_evt: Event, wsInstance: WSContext) => {
        webSocketManager.handleConnectionOpen(wsInstance);
      },
      onMessage: (evt: MessageEvent, wsInstance: WSContext) => {
        webSocketManager.handleMessage(wsInstance, evt.data);
      },
      onClose: (_evt: CloseEvent, wsInstance: WSContext) => {
        webSocketManager.handleConnectionClose(wsInstance);
      },
      onError: (evt: Event, wsInstance: WSContext) => {
        logger.error("WebSocket error event:", evt);
        const error = (evt as ErrorEvent).error || new Error("WebSocket error");
        webSocketManager.handleConnectionError(wsInstance, error);
      },
    };
  })
);

// Export fetch and websocket handlers for Bun
default {
  fetch: app.fetch,
  websocket, // Add the websocket handler
};

} // namespace elizaos
