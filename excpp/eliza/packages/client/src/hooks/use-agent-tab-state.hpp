#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Key for storing agent tab states in localStorage

using TabValue = std::variant<'details', 'actions', 'logs', 'memories', std::string>;

struct AgentTabStates {
};

/**
 * Custom hook to manage agent sidebar tab state with localStorage persistence
 * Each agent remembers its last selected tab when switching between agents
 */
void useAgentTabState(UUID agentId);

} // namespace elizaos
