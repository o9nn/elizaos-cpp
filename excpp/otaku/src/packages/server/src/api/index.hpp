#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "agents.hpp"
#include "audio.hpp"
#include "auth.hpp"
#include "cdp.hpp"
#include "elizaos/core.hpp"
#include "entities.hpp"
#include "media.hpp"
#include "memory.hpp"
#include "messaging.hpp"
#include "middleware.hpp"
#include "runtime.hpp"
#include "socketio.hpp"
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

SocketIOServer setupSocketIO(http::Server server, ElizaOS elizaOS, AgentServer serverInstance);

// Setup log streaming integration with the logger
void setupLogStreaming(SocketIOServer io, SocketIORouter router);

// Extracted function to handle plugin routes

/**
 * Creates an API router with various endpoints and middleware.
 * @param {ElizaOS} elizaOS - ElizaOS instance containing all agents and their runtimes.
 * @param {AgentServer} [server] - Optional AgentServer instance.
 * @returns {express.Router} The configured API router.
 */

} // namespace elizaos
