#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct UseSocketChatProps {
    UUID channelId;
    std::string currentUserId;
    UUID contextId;
    std::variant<ChannelType::DM, ChannelType::GROUP> chatType;
    std::vector<Agent> allAgents;
    std::vector<UiMessage> messages;
};

void useSocketChat(auto currentUserId, auto contextId, auto chatType, auto allAgents, auto messages, auto onAddMessage, auto onUpdateMessage, auto onDeleteMessage, auto onClearMessages, auto onInputDisabledChange);

} // namespace elizaos
