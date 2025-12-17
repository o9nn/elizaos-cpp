#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include "chat.hpp"
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct ChatMessageListComponentProps {
    std::vector<UiMessage> messages;
    bool isLoadingMessages;
    std::variant<ChannelType::GROUP, ChannelType::DM> chatType;
    std::string currentClientEntityId;
    std::optional<Agent> targetAgentData;
    std::vector<std::optional<Agent>> allAgents;
    std::string animatedMessageId;
    std::optional<std::shared_ptr<HTMLDivElement>> scrollRef;
    std::optional<std::optional<std::shared_ptr<HTMLDivElement>>> contentRef;
    bool isAtBottom;
    UUID finalChannelId;
    std::optional<std::optional<UUID>> selectedGroupAgentId;
};

  // Filter messages based on selected agent in group chat
        // Show user messages and messages from selected agent


} // namespace elizaos
