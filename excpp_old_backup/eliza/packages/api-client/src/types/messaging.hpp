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
    std::string name;
    std::string sourceType;
    std::optional<std::string> sourceId;
    std::optional<std::unordered_map<std::string, std::any>> metadata;
    Date createdAt;
    Date updatedAt;
};

struct MessageChannel {
    UUID id;
    UUID messageServerId;
    std::string name;
    ChannelType type;
    std::optional<std::string> sourceType;
    std::optional<std::string> sourceId;
    std::optional<std::string> topic;
    std::optional<std::unordered_map<std::string, std::any>> metadata;
    Date createdAt;
    Date updatedAt;
};

struct Message {
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
    std::optional<std::unordered_map<std::string, std::any>> metadata;
};

struct MessageSubmitParams {
    UUID agentId;
    UUID channelId;
    std::string content;
    std::optional<UUID> inReplyToMessageId;
    std::optional<std::unordered_map<std::string, std::any>> metadata;
};

struct MessageCompleteParams {
    UUID messageId;
    'completed' | 'failed' status;
    std::optional<std::string> error;
};

struct ExternalMessageParams {
    std::string platform;
    std::string channelId;
    Array<{ messages;
    std::string id;
    std::string authorId;
    std::string content;
    double timestamp;
    std::optional<std::unordered_map<std::string, std::any>> metadata;
};

struct ChannelCreateParams {
    std::string name;
    ChannelType type;
    std::optional<UUID> serverId;
    std::optional<std::unordered_map<std::string, std::any>> metadata;
};

struct GroupChannelCreateParams {
    std::string name;
    std::vector<UUID> participantIds;
    std::optional<std::unordered_map<std::string, std::any>> metadata;
};

struct DmChannelParams {
    [UUID, UUID] participantIds;
};

struct ChannelParticipant {
    UUID id;
    UUID channelId;
    UUID userId;
    std::optional<std::string> role;
    Date joinedAt;
};

struct ServerCreateParams {
    std::string name;
    std::string sourceType;
    std::optional<std::string> sourceId;
    std::optional<std::unordered_map<std::string, std::any>> metadata;
};

struct ServerSyncParams {
    Array<{ channels;
    std::string name;
    ChannelType type;
    std::string sourceId;
};

struct ChannelUpdateParams {
    std::optional<std::string> name;
    std::optional<std::vector<UUID>> participantCentralUserIds;
    std::optional<std::unordered_map<std::string, std::any>> metadata;
};


} // namespace elizaos
