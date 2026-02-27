#include "agent-memory-edit-overlay.hpp"
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



// Number of items to load per batch

struct MemoryContent {
    std::optional<boolean | string> thought;
    std::optional<std:> channelType;
    std::optional<std:> source;
    std::optional<std:> text;
    std::optional<{> metadata;
    std::optional<std:> fileType;
    std::optional<std:> title;
    std::optional<std:> filename;
    std::optional<std:> path;
    std::optional<std:> description;
};

enum MemoryType {
  all = 'all',
  currentChat = 'currentChat',
  messagesReceived = 'messagesReceived',
  messagesSent = 'messagesSent',
  facts = 'facts',
}

struct AgentMemoryViewerProps {
    UUID agentId;
    std: agentName;
    std::optional<UUID; // Renamed from roomId to channelId for clarity> channelId;
};

void AgentMemoryViewer(auto { agentId, auto agentName, AgentMemoryViewerProps channelId });

} // namespace elizaos
