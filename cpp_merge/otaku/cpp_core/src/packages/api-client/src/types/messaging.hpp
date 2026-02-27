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
    std::optional<std:> description;
    std::optional<std:> icon;
    std::optional<std:> adminId;
};

/**
 * Channel metadata interface
 */
struct ChannelMetadata {
    std::optional<std:> description;
    std::optional<std:> topic;
    std::optional<std::vector<std::string>> participants;
    std::optional<UUID[]; // Used by messaging service> participantCentralUserIds;
    std::optional<bool> isPrivate;
    std::optional<std:> sessionId;
    std::optional<std:> agentId;
    std::optional<std:> userId;
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
    std::optional<std:> agentName;
    std::optional<std:> thought;
    std::optional<std::vector<std::string>> actions;
    std::optional<Array<{> attachments;
    std: id;
    std: url;
    std::optional<std:> type;
    std::optional<std:> title;
    std::optional<std:> source;
    std::optional<std:> description;
    std::optional<std:> text;
    std::optional<std:> contentType;
    std::optional<std:> name;
    std::optional<double> size;
    std::optional<std:> authorDisplayName;
    std::optional<UUID> serverId;
    std::optional<std:> prompt;
    std::optional<std:> source;
    std::optional<'low' | 'normal' | 'high'> priority;
    std::optional<std::vector<std::string>> tags;
    std::optional<std::unordered_map<std:, std: | number | boolean>> context;
    std::optional<std:> serverName;
    std::optional<std:> channelName;
    std::optional<std:> channelType;
    std::optional<std::unordered_map<std:, unknown>> serverMetadata;
    std::optional<std::unordered_map<std:, unknown>> channelMetadata;
    std::optional<bool> isDm;
    std::optional<UUID> agent_id;
};

/**
 * External message metadata interface
 */
struct ExternalMessageMetadata {
    std::optional<std:> platform;
    std::optional<std:> externalId;
    std::optional<double> timestamp;
    std::optional<bool> edited;
    std::optional<Array<{> reactions;
    std: emoji;
    double count;
    std::vector<std::string> users;
};

struct MessageServer {
    UUID id;
    std: name;
    std: sourceType;
    std::optional<std:> sourceId;
    std::optional<ServerMetadata> metadata;
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
    std::optional<ChannelMetadata> metadata;
    Date createdAt;
    Date updatedAt;
};

struct Message {
    UUID id;
    UUID channelId;
    UUID authorId;
    std: content;
    std::optional<unknown> rawMessage;
    std::optional<UUID> inReplyToRootMessageId;
    std::optional<std:> sourceType;
    std::optional<std:> sourceId;
    Date createdAt;
    Date updatedAt;
    std::optional<MessageMetadata> metadata;
};

struct MessageSubmitParams {
    UUID agentId;
    UUID channelId;
    std: content;
    std::optional<UUID> inReplyToMessageId;
    std::optional<MessageMetadata> metadata;
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
    std::optional<ExternalMessageMetadata> metadata;
};

struct ChannelCreateParams {
    std: name;
    ChannelType type;
    std::optional<UUID> serverId;
    std::optional<ChannelMetadata> metadata;
};

struct GroupChannelCreateParams {
    std: name;
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
    std::optional<std:> role;
    Date joinedAt;
};

struct ServerCreateParams {
    std::optional<UUID> id;
    std: name;
    std: sourceType;
    std::optional<std:> sourceId;
    std::optional<ServerMetadata> metadata;
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
    std::optional<ChannelMetadata> metadata;
};


} // namespace elizaos
