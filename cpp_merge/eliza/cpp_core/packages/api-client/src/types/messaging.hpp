#include "base.hpp"
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



struct MessageServer {
    UUID id;
    std: name;
    std: sourceType;
    std::optional<std:> sourceId;
    std::optional<std::unordered_map<std:, std:>> metadata;
    Date createdAt;
    Date updatedAt;
};

struct MessageChannel {
    UUID id;
    UUID messageServerId;
    std: name;
    ChannelType type;
    std::optional<std:> sourceType;
    std::optional<std:> sourceId;
    std::optional<std:> topic;
    std::optional<std::unordered_map<std:, std:>> metadata;
    Date createdAt;
    Date updatedAt;
};

struct Message {
    UUID id;
    UUID channelId;
    UUID authorId;
    std: content;
    std::optional<std:> rawMessage;
    std::optional<UUID> inReplyToRootMessageId;
    std::optional<std:> sourceType;
    std::optional<std:> sourceId;
    Date createdAt;
    Date updatedAt;
    std::optional<std::unordered_map<std:, std:>> metadata;
};

struct MessageSubmitParams {
    UUID agentId;
    UUID channelId;
    std: content;
    std::optional<UUID> inReplyToMessageId;
    std::optional<std::unordered_map<std:, std:>> metadata;
};

struct MessageCompleteParams {
    UUID messageId;
    'completed' | 'failed' status;
    std::optional<std:> error;
};

struct ExternalMessageParams {
    std: platform;
    std: channelId;
    Array<{ messages;
    std: id;
    std: authorId;
    std: content;
    double timestamp;
    std::optional<std::unordered_map<std:, std:>> metadata;
};

struct ChannelCreateParams {
    std: name;
    ChannelType type;
    std::optional<UUID> serverId;
    std::optional<std::unordered_map<std:, std:>> metadata;
};

struct GroupChannelCreateParams {
    std: name;
    std::vector<UUID> participantIds;
    std::optional<std::unordered_map<std:, std:>> metadata;
};

struct DmChannelParams {
    [UUID, UUID] participantIds;
};

struct ChannelParticipant {
    UUID id;
    UUID channelId;
    UUID userId;
    std::optional<std:> role;
    Date joinedAt;
};

struct ServerCreateParams {
    std: name;
    std: sourceType;
    std::optional<std:> sourceId;
    std::optional<std::unordered_map<std:, std:>> metadata;
};

struct ServerSyncParams {
    Array<{ channels;
    std: name;
    ChannelType type;
    std: sourceId;
};

struct ChannelUpdateParams {
    std::optional<std:> name;
    std::optional<std::vector<UUID>> participantCentralUserIds;
    std::optional<std::unordered_map<std:, std:>> metadata;
};


} // namespace elizaos
