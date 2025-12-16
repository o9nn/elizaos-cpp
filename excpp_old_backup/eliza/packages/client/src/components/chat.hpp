#include "ChatInputArea.hpp"
#include "ChatMessageListComponent.hpp"
#include "agent-sidebar.hpp"
#include "elizaos/core.hpp"
#include "group-panel.hpp"
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



struct UnifiedChatViewProps {
    ChannelType.DM | ChannelType.GROUP chatType;
    UUID; // agentId for DM, channelId for GROUP contextId;
    std::optional<UUID; // Required for GROUP, optional for DM> serverId;
    std::optional<UUID; // New prop for specific DM channel from URL> initialDmChannelId;
};

// Consolidated chat state type
struct ChatUIState {
    bool showGroupEditPanel;
    bool showProfileOverlay;
    std::string input;
    bool inputDisabled;
    UUID | null selectedGroupAgentId;
    UUID | null currentDmChannelId;
    bool isCreatingDM;
    boolean; // Add mobile state isMobile;
};

// Message content component - exported for use in ChatMessageListComponent
  // Only re-render if the message content, animation state, or other key props change


} // namespace elizaos
