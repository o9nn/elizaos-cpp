#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "character.hpp"
#include "elizaos/core.hpp"
#include "elizaos/plugin-autocoder.hpp"
#include "elizaos/plugin-autonomy.hpp"
#include "elizaos/plugin-bootstrap.hpp"
#include "elizaos/plugin-experience.hpp"
#include "elizaos/plugin-goals.hpp"
#include "elizaos/plugin-inference.hpp"
#include "elizaos/plugin-knowledge.hpp"
#include "elizaos/plugin-personality.hpp"
#include "elizaos/plugin-plugin-manager.hpp"
#include "elizaos/plugin-sam.hpp"
#include "elizaos/plugin-shell.hpp"
#include "elizaos/plugin-sql.hpp"
#include "elizaos/plugin-stagehand.hpp"
#include "elizaos/plugin-todo.hpp"
#include "elizaos/plugin-vision.hpp"
#include "game-api-plugin.hpp"
#include "server.hpp"
#include "services/capabilityProgressionService.hpp"
#include "services/progressionTracker.hpp"
#include "services/progressivePluginService.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Agent ID Handling Strategy:
 *
 * The system supports multiple ways to access agent-specific endpoints:
 *
 * 1. Using "default" as agent ID - This resolves to the first available agent
 *    Example: /api/agents/default/settings
 *
 * 2. Using actual agent UUID - Direct access to a specific agent
 *    Example: /api/agents/123e4567-e89b-12d3-a456-426614174000/settings
 *
 * 3. Discovery endpoints:
 *    - GET /api/agents/primary - Returns the primary (first) agent's details
 *    - GET /api/agents - Returns list of all available agents
 *
 * Frontend should ideally:
 * 1. Call /api/agents/primary to get the actual agent ID
 * 2. Use the returned agent ID for subsequent API calls
 * 3. Fall back to "default" if needed for backward compatibility
 */

// Helper to load .env only if file exists
void loadEnvIfExists(const std::string& envPath);

// Check which embedding provider to use

// Define all available plugins for progressive unlocking

// Create initial plugin list with only basic capabilities
std::vector<Plugin> createInitialPlugins();

// Function to start an agent runtime - called by server.ts
std::future<IAgentRuntime> startAgent(Character character);

std::future<void> startServer();

// Start the server only if this file is run directly (not imported)

} // namespace elizaos
