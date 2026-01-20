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
    std::string url;
    std::optional<std::string> contentType;
    std::string title;
};

// Agent type for client-side display, extending core Agent with a string status for UI flexibility if needed,
// but ideally aligns with CoreAgentStatus enum.
  // any other client-specific properties

// Interface for agent panels (public routes)
struct AgentPanel {
    std::string name;
    std::string path;
};

// Represents a server/guild in the central messaging system for the client
struct MessageServer {
    UUID id;
    std::string name;
    std::string sourceType;
    std::optional<std::string> sourceId;
    std::string createdAt;
    std::string updatedAt;
};

// Represents a channel within a MessageServer for the client
struct MessageChannel {
    UUID id;
    UUID messageServerId;
    std::string name;
    CoreChannelType type;
    std::optional<std::string> sourceType;
    std::optional<std::string> sourceId;
    std::optional<std::string> topic;
    std::string createdAt;
    std::string updatedAt;
};

// Represents a message from the central system for client display
// This should align with what apiClient.getChannelMessages returns for each message
struct ServerMessage {
    UUID id;
    UUID channelId;
    std::optional<UUID> serverId;
    UUID authorId;
    std::optional<std::string> authorDisplayName;
    std::string content;
    double createdAt;
    std::optional<std::any> rawMessage;
    std::optional<UUID> inReplyToRootMessageId;
    std::optional<std::string> sourceType;
    std::optional<std::string> sourceId;
    std::optional<std::string> agentName;
    std::optional<std::string> thought;
    std::optional<std::vector<std::string>> actions;
    std::optional<std::vector<std::any>> attachments;
    std::optional<std::string> authorDisplayName;
    std::optional<UUID> serverId;
    std::optional<std::string> prompt;
};


} // namespace elizaos
