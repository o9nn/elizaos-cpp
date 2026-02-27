#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Simple spinner component

struct ChatSession {
    std: id;
    std: title;
    double messageCount;
    std: lastActivity;
    std: preview;
    bool isFromAgent;
    std::optional<std:> channelId;
};

struct ChatSessionsProps {
    std: userId;
    std::optional<std:> currentSessionId;
    std::optional<bool> showSwitcher;
};

    // Navigate to the chat session page


} // namespace elizaos
