#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>
#include "ChatInputArea.hpp"
#include "ChatMessageListComponent.hpp"
#include "agent-sidebar.hpp"
#include "elizaos/core.hpp"
#include "group-panel.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct UnifiedChatViewProps {
    std::variant<ChannelType::DM, ChannelType::GROUP> chatType;
    UUID contextId;
    std::optional<UUID> serverId;
    std::optional<UUID> initialDmChannelId;
};

// Consolidated chat state type
struct ChatUIState {
    bool showGroupEditPanel;
    bool showProfileOverlay;
    std::string input;
    bool inputDisabled;
    std::optional<UUID> selectedGroupAgentId;
    std::optional<UUID> currentDmChannelId;
    bool isCreatingDM;
    bool isMobile;
};

// Message content component - exported for use in ChatMessageListComponent
  // Only re-render if the message content, animation state, or other key props change


} // namespace elizaos
