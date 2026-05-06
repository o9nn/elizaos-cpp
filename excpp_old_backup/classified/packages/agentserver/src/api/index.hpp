#include "agents.hpp"
#include "audio.hpp"
#include "elizaos/core.hpp"
#include "media.hpp"
#include "memory.hpp"
#include "messaging.hpp"
#include "runtime.hpp"
#include "shared/middleware.hpp"
#include "system.hpp"
#include "tee.hpp"
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



// Import new domain routers

// NOTE: world router has been removed - functionality moved to messaging/spaces
// NOTE: goals and todos routers removed - handled by plugins directly

// Extracted function to handle plugin routes

/**
 * Creates an API router with various endpoints and middleware.
 * @param {Map<UUID, IAgentRuntime>} agents - Map of agents with UUID as key and IAgentRuntime as value.
 * @param {AgentServer} [server] - Optional AgentServer instance.
 * @returns {express.Router} The configured API router.
 */

} // namespace elizaos
