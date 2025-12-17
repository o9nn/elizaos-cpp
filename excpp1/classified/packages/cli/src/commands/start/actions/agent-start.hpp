#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".types.hpp"
#include ".utils/config-utils.hpp"
#include ".utils/dependency-resolver.hpp"
#include ".utils/plugin-utils.hpp"
#include "elizaos/core.hpp"
#include "elizaos/plugin-sql.hpp"
#include "elizaos/server.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Start an agent with the given configuration
 *
 * Creates and initializes an agent runtime with plugins, handles dependency resolution, runs database migrations, and registers the agent with the server.
 */

  // Handle secrets for character configuration

  // Type-cast to ensure compatibility with local types

  // Load all requested plugins

  // Resolve dependencies and get final plugin list

  // Discover and run plugin schema migrations

/**
 * Stop an agent and unregister it from the server
 */
std::future<void> stopAgent(IAgentRuntime runtime, AgentServer server);

} // namespace elizaos
