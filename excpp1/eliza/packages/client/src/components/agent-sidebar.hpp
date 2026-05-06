#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include "agent-action-viewer.hpp"
#include "agent-log-viewer.hpp"
#include "agent-memory-viewer.hpp"
#include "ui/skeleton.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



using AgentSidebarProps = std::variant<{

using FixedTabValue = std::variant<'details', 'actions', 'logs', 'memories'>;
using TabValue = std::variant<FixedTabValue, std::string>;

void AgentSidebar(auto { agentId, auto agentName, AgentSidebarProps channelId });

} // namespace elizaos
