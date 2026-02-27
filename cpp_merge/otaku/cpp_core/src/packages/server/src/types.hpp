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
    std: name;
    std: sourceType;
    std::optional<std:> sourceId;
    std::optional<ServerMetadata> metadata;
    std::chrono::system_clock::time_point createdAt;
    std::chrono::system_clock::time_point updatedAt;
};

struct MessageChannel {
    UUID id;
    UUID messageServerId;
    std: name;
    ChannelType type;
    std::optional<std:> sourceType;
    std::optional<std:> sourceId;
    std::optional<std:> topic;
    std::optional<ChannelMetadata> metadata;
    std::chrono::system_clock::time_point createdAt;
    std::chrono::system_clock::time_point updatedAt;
};

struct CentralRootMessage {
    UUID id;
    UUID channelId;
    UUID authorId;
    std: content;
    std::optional<std:> rawMessage;
    std::optional<UUID> inReplyToRootMessageId;
    std::optional<std:> sourceType;
    std::optional<std:> sourceId;
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
    std::optional<std:> author_display_name;
    std: content;
    std::optional<std:> raw_message;
    std::optional<std:> source_id;
    std::optional<std:> source_type;
    std::optional<UUID> in_reply_to_message_id;
    double created_at;
    std::optional<MessageMetadata> metadata;
};

// Attachment types for media transformation
struct Attachment {
    std::optional<std:> url;
};

using AttachmentInput = std::variant<std:, Attachment, (std:, std::vector<Attachment)>>;

struct MessageContentWithAttachments {
    std::optional<AttachmentInput> attachments;
};

struct MessageMetadataWithAttachments {
    std::optional<AttachmentInput> attachments;
};

struct MessageWithAttachments {
    std::optional<std::variant<MessageContentWithAttachments, std:>> content;
    std::optional<MessageMetadataWithAttachments> metadata;
};

// Re-session types
* from './types/sessions';

} // namespace elizaos
