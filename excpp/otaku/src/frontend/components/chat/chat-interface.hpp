#include ".ui/textarea.hpp"
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



// Plugin definitions with metadata and sample prompts

// Helper function to extract chart data from a message

// Helper function to find all chart data in an action group

struct Message {
    std::string id;
    std::string content;
    std::string authorId;
    double createdAt;
    bool isAgent;
    std::optional<std::string> senderName;
    std::optional<std::string> sourceType;
    std::optional<std::string> type;
    std::optional<std::any> rawMessage;
    std::optional<std::any> metadata;
    std::optional<std::string> thought;
};

struct ChatInterfaceProps {
    Agent agent;
    std::string userId;
    std::string serverId;
    string | null channelId;
    std::optional<bool> isNewChatMode;
    std::optional<(channelId: string, channelName: string) => void> onChannelCreated;
    std::optional<() => void // Callback when agent completes an action> onActionCompleted;
};

void ChatInterface(auto { agent, auto userId, auto serverId, auto channelId, auto isNewChatMode = false, auto onChannelCreated, ChatInterfaceProps onActionCompleted });

} // namespace elizaos
