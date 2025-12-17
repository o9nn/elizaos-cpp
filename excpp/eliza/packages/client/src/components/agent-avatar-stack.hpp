#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "elizaos/core.hpp"
#include "ui/avatar.hpp"
#include "ui/tooltip.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct AgentAvatarStackProps {
    std::vector<UUID> agentIds;
    std::vector<std::string> agentNames;
    std::optional<std::string> size;
    std::optional<double> maxStack;
    std::optional<bool> showExtraTooltip;
};


} // namespace elizaos
