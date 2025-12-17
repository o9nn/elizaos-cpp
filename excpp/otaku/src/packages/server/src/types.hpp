#pragma once
#include <algorithm>
#include <any>
#include <chrono>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use


struct MessageServer {
    UUID id;
    std::string name;
    std::string sourceType;
    std::optional<std::string> sourceId;
    std::optional<ServerMetadata> metadata;
    std::chrono::system_clock::time_point createdAt;
    std::chrono::system_clock::time_point updatedAt;
};

struct MessageChannel {
    UUID id;
    UUID messageServerId;
    std::string name;
    ChannelType type;
    std::optional<std::string> sourceType;
    std::optional<std::string> sourceId;
    std::optional<std::string> topic;
    std::optional<ChannelMetadata> metadata;
    std::chrono::system_clock::time_point createdAt;
    std::chrono::system_clock::time_point updatedAt;
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
    std::chrono::system_clock::time_point createdAt;
    std::chrono::system_clock::time_point updatedAt;
    std::optional<MessageMetadata> metadata;
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
    std::optional<MessageMetadata> metadata;
};

// Attachment types for media transformation
struct Attachment {
    std::optional<std::string> url;
};

using AttachmentInput = std::variant<std::string, Attachment, (string, std::vector<Attachment)>>;

struct MessageContentWithAttachments {
    std::optional<AttachmentInput> attachments;
};

struct MessageMetadataWithAttachments {
    std::optional<AttachmentInput> attachments;
};

struct MessageWithAttachments {
    std::optional<std::variant<MessageContentWithAttachments, std::any>> content;
    std::optional<MessageMetadataWithAttachments> metadata;
};

// Re-session types
* from './types/sessions';

} // namespace elizaos
