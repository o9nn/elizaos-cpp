#pragma once
#include <any>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "base.hpp"
#include "elizaos/core.hpp"

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
    UUID messageServerId;
    std::string name;
    ChannelType type;
    std::optional<std::string> sourceType;
    std::optional<std::string> sourceId;
    std::optional<std::string> topic;
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
};

struct MessageSubmitParams {
    UUID agentId;
    UUID channelId;
    std::string content;
    std::optional<UUID> inReplyToMessageId;
};

struct MessageCompleteParams {
    UUID messageId;
    std::string status;
    std::optional<std::string> error;
};

struct ExternalMessageParams {
    std::string platform;
    std::string channelId;
    std::string id;
    std::string authorId;
    std::string content;
    double timestamp;
};

struct ChannelCreateParams {
    std::string name;
    ChannelType type;
    std::optional<UUID> serverId;
};

struct GroupChannelCreateParams {
    std::string name;
    std::vector<UUID> participantIds;
};

struct DmChannelParams {
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
};

struct ServerSyncParams {
    std::string name;
    ChannelType type;
    std::string sourceId;
};

struct ChannelUpdateParams {
    std::optional<std::string> name;
    std::optional<std::vector<UUID>> participantCentralUserIds;
};


} // namespace elizaos
