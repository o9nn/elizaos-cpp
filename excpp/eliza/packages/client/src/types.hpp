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
    std::optional<string; // Make contentType optional> contentType;
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
    UUID; // Global serverId id;
    std::string name;
    std::string sourceType;
    std::optional<std::string> sourceId;
    std::optional<std::unordered_map<std::string, std::any>> metadata;
    string; // ISO Date string from server, or Date object createdAt;
    string; // ISO Date string from server, or Date object updatedAt;
};

// Represents a channel within a MessageServer for the client
struct MessageChannel {
    UUID; // Global channelId id;
    UUID messageServerId;
    std::string name;
    CoreChannelType; // Using the enum from @elizaos/core type;
    std::optional<std::string> sourceType;
    std::optional<std::string> sourceId;
    std::optional<std::string> topic;
    std::optional<std::unordered_map<std::string, std::any>> metadata;
    string; // ISO Date string from server, or Date object createdAt;
    string; // ISO Date string from server, or Date object updatedAt;
};

// Represents a message from the central system for client display
// This should align with what apiClient.getChannelMessages returns for each message
struct ServerMessage {
    UUID id;
    UUID channelId;
    std::optional<UUID; // Optional: May be added during client-side processing or be in metadata> serverId;
    UUID authorId;
    std::optional<string; // Optional: May be in metadata or fetched separately> authorDisplayName;
    std::string content;
    number; // Expecting timestamp MS for UI sorting/display createdAt;
    std::optional<std::any> rawMessage;
    std::optional<UUID> inReplyToRootMessageId;
    std::optional<std::string> sourceType;
    std::optional<std::string> sourceId;
    std::optional<{> metadata;
    std::optional<std::string> agentName;
    std::optional<std::string> thought;
    std::optional<std::vector<std::string>> actions;
    std::optional<std::vector<std::any>> attachments;
    std::optional<string; // If API puts it here> authorDisplayName;
    std::optional<UUID; // If API puts it here> serverId;
    std::optional<string; // Add prompt field to store the LLM prompt> prompt;
};


} // namespace elizaos
