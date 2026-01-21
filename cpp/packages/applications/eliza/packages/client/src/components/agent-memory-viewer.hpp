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
    std::optional<std::string> channelType;
    std::optional<std::string> source;
    std::optional<std::string> text;
    std::optional<{> metadata;
    std::optional<std::string> fileType;
    std::optional<std::string> title;
    std::optional<std::string> filename;
    std::optional<std::string> path;
    std::optional<std::string> description;
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
    std::string agentName;
    std::optional<UUID; // Renamed from roomId to channelId for clarity> channelId;
};

void AgentMemoryViewer(auto { agentId, auto agentName, AgentMemoryViewerProps channelId });

} // namespace elizaos
