#include "chat.hpp"
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



struct ChatMessageListComponentProps {
    std::vector<UiMessage> messages;
    bool isLoadingMessages;
    ChannelType.GROUP | ChannelType.DM chatType;
    std::string currentClientEntityId;
    std::optional<Agent> targetAgentData;
    std::vector<std::optional<Agent>> allAgents;
    string | null animatedMessageId;
    React.RefObject<HTMLDivElement | null> scrollRef;
    std::optional<React.RefObject<HTMLDivElement | null>; // Optional content ref for StickToBottom> contentRef;
    bool isAtBottom;
    () => void scrollToBottom;
    () => void disableAutoScroll;
    UUID | undefined finalChannelId;
    std::optional<(agentId: UUID) => Partial<Agent> | undefined> getAgentInMessage;
    std::optional<std::unordered_map<UUID, string | null>> agentAvatarMap;
    (messageId: string) => void onDeleteMessage;
    (messageText: string) => void onRetryMessage;
    std::optional<UUID | null> selectedGroupAgentId;
};

  // Filter messages based on selected agent in group chat
        // Show user messages and messages from selected agent


} // namespace elizaos
