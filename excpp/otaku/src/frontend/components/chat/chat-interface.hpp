#pragma once
#include <algorithm>
#include <any>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "ui/textarea.hpp"

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
    std::string channelId;
    std::optional<bool> isNewChatMode;
};

void ChatInterface(auto userId, auto serverId, auto channelId, auto isNewChatMode, auto onChannelCreated);

} // namespace elizaos
