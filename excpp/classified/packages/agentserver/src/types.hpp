#pragma once
#include <any>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use


struct MessageServer {
    UUID id;
    std::string name;
    std::string sourceType;
    std::optional<std::string> sourceId;
    Date createdAt;
    Date updatedAt;
};

struct MessageChannel {
    UUID id;
    UUID serverId;
    std::string name;
    ChannelType type;
    std::optional<std::string> sourceType;
    std::optional<std::string> sourceId;
    std::optional<std::string> topic;
    Date createdAt;
    Date updatedAt;
};

struct CentralRootMessage {
    UUID id;
    UUID channelId;
    UUID authorId;
    std::string content;
    std::optional<std::any> rawMessage;
    std::optional<UUID> inReplyToRootMessageId;
    std::optional<std::string> sourceType;
    std::optional<std::string> sourceId;
    Date createdAt;
    Date updatedAt;
};

// This is what goes on the internal bus and often what APIs might return for a "full" message
struct MessageServiceStructure {
    UUID id;
    UUID channel_id;
    UUID server_id;
    UUID author_id;
    std::optional<std::string> author_display_name;
    std::string content;
    std::optional<std::any> raw_message;
    std::optional<std::string> source_id;
    std::optional<std::string> source_type;
    std::optional<UUID> in_reply_to_message_id;
    double created_at;
    std::optional<std::any> metadata;
};

// Extend the core service types with message bus service
  struct ServiceTypeRegistry {
    std::string MESSAGE_BUS_SERVICE;
};

// Export service type constant

} // namespace elizaos
