#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".socketio.hpp"
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

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Import new domain routers

// NOTE: world router has been removed - functionality moved to messaging/spaces

/**
 * Sets up Socket.io server for real-time messaging
 * @param server HTTP Server instance
 * @param agents Map of agent runtimes
 */
// Global reference to SocketIO router for log streaming
// let socketIORouter: SocketIORouter | null = null; // This can be removed if router is managed within setupSocketIO scope correctly

SocketIOServer setupSocketIO(http::Server server, const std::unordered_map<UUID, IAgentRuntime>& agents, AgentServer serverInstance);

// Setup log streaming integration with the logger
void setupLogStreaming(SocketIOServer io, SocketIORouter router);

// Extracted function to handle plugin routes

/**
 * Creates an API router with various endpoints and middleware.
 * @param {Map<UUID, IAgentRuntime>} agents - Map of agents with UUID as key and IAgentRuntime as value.
 * @param {AgentServer} [server] - Optional AgentServer instance.
 * @returns {express.Router} The configured API router.
 */

} // namespace elizaos
