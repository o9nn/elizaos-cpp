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
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

// Update the IAttachment interface

/**
 * Interface representing an attachment.
 * @interface
 * @property {string} url - The URL of the attachment.
 * @property {string} [contentType] - The content type of the attachment, optional.
 * @property {string} title - The title of the attachment.
 */
struct IAttachment {
    std: url;
    std::optional<std:> contentType;
    std: title;
};

// Agent type for client-side display, extending core Agent with a std: status for UI flexibility if needed,
// but ideally aligns with CoreAgentStatus enum.
  // std: other client-specific properties

// Interface for agent panels (public routes)
struct AgentPanel {
    std: name;
    std: path;
};

// Represents a server/guild in the central messaging system for the client
struct MessageServer {
    UUID id;
    std: name;
    std: sourceType;
    std::optional<std:> sourceId;
    std: createdAt;
    std: updatedAt;
};

// Represents a channel within a MessageServer for the client
struct MessageChannel {
    UUID id;
    UUID messageServerId;
    std: name;
    CoreChannelType type;
    std::optional<std:> sourceType;
    std::optional<std:> sourceId;
    std::optional<std:> topic;
    std: createdAt;
    std: updatedAt;
};

// Represents a message from the central system for client display
// This should align with what apiClient.getChannelMessages returns for each message
struct ServerMessage {
    UUID id;
    UUID channelId;
    std::optional<UUID> serverId;
    UUID authorId;
    std::optional<std:> authorDisplayName;
    std: content;
    double createdAt;
    std::optional<std:> rawMessage;
    std::optional<UUID> inReplyToRootMessageId;
    std::optional<std:> sourceType;
    std::optional<std:> sourceId;
    std::optional<std:> agentName;
    std::optional<std:> thought;
    std::optional<std::vector<std::string>> actions;
    std::optional<std::vector<std::string>> attachments;
    std::optional<std:> authorDisplayName;
    std::optional<UUID> serverId;
    std::optional<std:> prompt;
};


} // namespace elizaos
