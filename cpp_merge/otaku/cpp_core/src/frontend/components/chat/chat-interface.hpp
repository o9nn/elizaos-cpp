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

// Helper std::function to extract chart data from a message

// Helper std::function to find all chart data in an action group

struct Message {
    std: id;
    std: content;
    std: authorId;
    double createdAt;
    bool isAgent;
    std::optional<std:> senderName;
    std::optional<std:> sourceType;
    std::optional<std:> type;
    std::optional<std:> rawMessage;
    std::optional<std:> metadata;
    std::optional<std:> thought;
};

struct ChatInterfaceProps {
    Agent agent;
    std: userId;
    std: serverId;
    std: | null channelId;
    std::optional<bool> isNewChatMode;
    std::optional<[&](channelId: std:, channelName: std:) { return void> onChannelCreated; };
    std::optional<[&]() { return void // Callback when agent completes an action> onActionCompleted; };
};

void ChatInterface(auto { agent, auto userId, auto serverId, auto channelId, auto isNewChatMode = false, auto onChannelCreated, ChatInterfaceProps onActionCompleted });

} // namespace elizaos
