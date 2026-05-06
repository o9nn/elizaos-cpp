#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
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

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



 // Import the cron task runner
 // Assuming Env type is defined and includes Redis vars

 // Import the new router

struct AppVariables {
};

// Setup Solana connection

// --- Middleware ---

// CORS Middleware (from original index.ts)

  // --- Mount the API sub-router ---

// --- Special Cron Trigger Route ---
// Use a non-standard path and require a secret header

// Mount this route directly on the main app, outside /api if desired

  // The lock mechanism inside runCronTasks will prevent overlaps.

  // Return immediately to the cron runner

// --- Root and Maintenance Routes ---

// --- Not Found Handler ---

// --- Error Handler ---
  // Avoid leaking stack traces in production

  // --- Initialize Services ---

  // Initialize WebSocketManager with Redis

// --- Create Bun WebSocket handlers ---
// --- Add WebSocket Upgrade Route ---

// Export fetch and websocket handlers for Bun

} // namespace elizaos
