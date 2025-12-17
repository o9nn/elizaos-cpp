#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "agent-start.hpp"
#include "elizaos/core.hpp"
#include "elizaos/server.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Server start options
 */
struct ServerStartOptions {
    std::optional<bool> configure;
    std::optional<double> port;
    std::optional<std::vector<Character>> characters;
    std::optional<std::vector<ProjectAgent>> projectAgents;
};

/**
 * Start the agents and server
 *
 * Initializes the database, creates the server instance, configures port settings, and starts the specified agents or default Eliza character.
 */
std::future<void> startAgents(ServerStartOptions options);

} // namespace elizaos
