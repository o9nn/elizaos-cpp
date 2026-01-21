#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>
#include "agent-action-viewer.hpp"
#include "agent-log-viewer.hpp"
#include "agent-memory-viewer.hpp"
#include "ui/skeleton.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



using AgentSidebarProps = {

using FixedTabValue = std::string;
using TabValue = std::variant<FixedTabValue, std::string>;

void AgentSidebar(auto agentName);

} // namespace elizaos
