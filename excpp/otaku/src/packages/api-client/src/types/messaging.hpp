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



/**
 * Server metadata interface for message servers
 */
struct ServerMetadata {
    std::optional<std::string> description;
    std::optional<std::string> icon;
    std::optional<std::string> adminId;
};

/**
 * Channel metadata interface
 */
struct ChannelMetadata {
    std::optional<std::string> description;
    std::optional<std::string> topic;
    std::optional<std::vector<std::string>> participants;
    std::optional<UUID[]; // Used by messaging service> participantCentralUserIds;
    std::optional<bool> isPrivate;
    std::optional<std::string> sessionId;
    std::optional<std::string> agentId;
    std::optional<std::string> userId;
    std::optional<{> timeoutConfig;
    std::optional<double> timeoutMinutes;
    std::optional<bool> autoRenew;
    std::optional<double> maxDurationMinutes;
    std::optional<double> warningThresholdMinutes;
};

/**
 * Message metadata interface
 */
struct MessageMetadata {
    std::optional<std::string> agentName;
    std::optional<std::string> thought;
    std::optional<std::vector<std::string>> actions;
    std::optional<Array<{> attachments;
    std::string id;
    std::string url;
    std::optional<std::string> type;
    std::optional<std::string> title;
    std::optional<std::string> source;
    std::optional<std::string> description;
    std::optional<std::string> text;
    std::optional<std::string> contentType;
    std::optional<std::string> name;
    std::optional<double> size;
    std::optional<std::string> authorDisplayName;
    std::optional<UUID> serverId;
    std::optional<std::string> prompt;
    std::optional<std::string> source;
    std::optional<'low' | 'normal' | 'high'> priority;
    std::optional<std::vector<std::string>> tags;
    std::optional<std::unordered_map<std::string, string | number | boolean>> context;
    std::optional<std::string> serverName;
    std::optional<std::string> channelName;
    std::optional<std::string> channelType;
    std::optional<std::unordered_map<std::string, unknown>> serverMetadata;
    std::optional<std::unordered_map<std::string, unknown>> channelMetadata;
    std::optional<bool> isDm;
    std::optional<UUID> agent_id;
};

/**
 * External message metadata interface
 */
struct ExternalMessageMetadata {
    std::optional<std::string> platform;
    std::optional<std::string> externalId;
    std::optional<double> timestamp;
    std::optional<bool> edited;
    std::optional<Array<{> reactions;
    std::string emoji;
    double count;
    std::vector<std::string> users;
};

struct MessageServer {
    UUID id;
    std::string name;
    std::string sourceType;
    std::optional<std::string> sourceId;
    std::optional<ServerMetadata> metadata;
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
    std::optional<ChannelMetadata> metadata;
    Date createdAt;
    Date updatedAt;
};

struct Message {
    UUID id;
    UUID channelId;
    UUID authorId;
    std::string content;
    std::optional<unknown> rawMessage;
    std::optional<UUID> inReplyToRootMessageId;
    std::optional<std::string> sourceType;
    std::optional<std::string> sourceId;
    Date createdAt;
    Date updatedAt;
    std::optional<MessageMetadata> metadata;
};

struct MessageSubmitParams {
    UUID agentId;
    UUID channelId;
    std::string content;
    std::optional<UUID> inReplyToMessageId;
    std::optional<MessageMetadata> metadata;
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
    std::optional<ExternalMessageMetadata> metadata;
};

struct ChannelCreateParams {
    std::string name;
    ChannelType type;
    std::optional<UUID> serverId;
    std::optional<ChannelMetadata> metadata;
};

struct GroupChannelCreateParams {
    std::string name;
    std::vector<UUID> participantIds;
    std::optional<ChannelMetadata> metadata;
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
    std::optional<UUID> id;
    std::string name;
    std::string sourceType;
    std::optional<std::string> sourceId;
    std::optional<ServerMetadata> metadata;
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
    std::optional<ChannelMetadata> metadata;
};


} // namespace elizaos
