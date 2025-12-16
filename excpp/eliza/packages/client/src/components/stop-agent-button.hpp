#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct StopAgentButtonProps {
    Agent agent;
    std::optional<'default' | 'destructive' | 'outline'> variant;
    std::optional<'default' | 'sm' | 'lg' | 'icon'> size;
    std::optional<std::string> className;
    std::optional<bool> showIcon;
    std::optional<bool> redirectToHome;
    std::optional<() => void> onStopComplete;
};


} // namespace elizaos
