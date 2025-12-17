#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "use-query-hooks.hpp"
#include "use-toast.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Direct error handling

/**
 * Custom hook for managing agents (starting, stopping, and tracking status)
 */
/**
 * Custom hook for agent management.
 * Allows starting and stopping agents with mutation operations.
 * Provides functions to check if an agent is currently starting or stopping.
 * @returns Object with functions for starting and stopping agents, checking agent status, and lists of agents in starting and stopping processes.
 */
void useAgentManagement();

} // namespace elizaos
