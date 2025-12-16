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



// Simple spinner component

struct ChatSession {
    std::string id;
    std::string title;
    double messageCount;
    std::string lastActivity;
    std::string preview;
    bool isFromAgent;
    std::optional<std::string> channelId;
};

struct ChatSessionsProps {
    string | null userId;
    std::optional<std::string> currentSessionId;
    std::optional<bool> showSwitcher;
};

    // Navigate to the chat session page


} // namespace elizaos
