#include "elizaos/core.hpp"
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



struct UseSocketChatProps {
    UUID | undefined channelId;
    std::string currentUserId;
    UUID; // agentId for DM, channelId for GROUP contextId;
    ChannelType.DM | ChannelType.GROUP chatType;
    std::vector<Agent> allAgents;
    std::vector<UiMessage> messages;
    (message: UiMessage) => void onAddMessage;
    (messageId: string, updates: Partial<UiMessage>) => void onUpdateMessage;
    (messageId: string) => void onDeleteMessage;
    () => void onClearMessages;
    (disabled: boolean) => void onInputDisabledChange;
};


} // namespace elizaos
