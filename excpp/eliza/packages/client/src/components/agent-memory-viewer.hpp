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
#include "agent-memory-edit-overlay.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Number of items to load per batch

struct MemoryContent {
    std::optional<std::variant<bool, std::string>> thought;
    std::optional<std::string> channelType;
    std::optional<std::string> source;
    std::optional<std::string> text;
    std::optional<std::string> fileType;
    std::optional<std::string> title;
    std::optional<std::string> filename;
    std::optional<std::string> path;
    std::optional<std::string> description;
};

enum MemoryType {
}

struct AgentMemoryViewerProps {
    UUID agentId;
    std::string agentName;
    std::optional<UUID> channelId;
};

void AgentMemoryViewer(auto agentName);

} // namespace elizaos
