#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_API_CLIENT_SRC_TYPES_MESSAGING_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_API_CLIENT_SRC_TYPES_MESSAGING_H
#include "core.hpp"
// External dependency removed
#include "./base.h"

class MessageServer;
class MessageChannel;
class Message;
class MessageSubmitParams;
class MessageCompleteParams;
class ExternalMessageParams;
class ChannelCreateParams;
class GroupChannelCreateParams;
class DmChannelParams;
class ChannelParticipant;
class MessageSearchParams;
class ServerCreateParams;
class ServerSyncParams;
class ChannelUpdateParams;

class MessageServer : public object, public std::enable_shared_from_this<MessageServer> {
public:
    using std::enable_shared_from_this<MessageServer>::shared_from_this;
    std::shared_ptr<UUID> id;

    string name;

    string sourceType;

    string sourceId;

    Record<string, any> metadata;

    std::shared_ptr<Date> createdAt;

    std::shared_ptr<Date> updatedAt;
};

class MessageChannel : public object, public std::enable_shared_from_this<MessageChannel> {
public:
    using std::enable_shared_from_this<MessageChannel>::shared_from_this;
    std::shared_ptr<UUID> id;

    std::shared_ptr<UUID> messageServerId;

    string name;

    std::shared_ptr<ChannelType> type;

    string sourceType;

    string sourceId;

    string topic;

    Record<string, any> metadata;

    std::shared_ptr<Date> createdAt;

    std::shared_ptr<Date> updatedAt;
};

class Message : public object, public std::enable_shared_from_this<Message> {
public:
    using std::enable_shared_from_this<Message>::shared_from_this;
    std::shared_ptr<UUID> id;

    std::shared_ptr<UUID> channelId;

    std::shared_ptr<UUID> authorId;

    string content;

    any rawMessage;

    std::shared_ptr<UUID> inReplyToRootMessageId;

    string sourceType;

    string sourceId;

    std::shared_ptr<Date> createdAt;

    std::shared_ptr<Date> updatedAt;

    Record<string, any> metadata;
};

class MessageSubmitParams : public object, public std::enable_shared_from_this<MessageSubmitParams> {
public:
    using std::enable_shared_from_this<MessageSubmitParams>::shared_from_this;
    std::shared_ptr<UUID> agentId;

    std::shared_ptr<UUID> channelId;

    string content;

    std::shared_ptr<UUID> inReplyToMessageId;

    Record<string, any> metadata;
};

class MessageCompleteParams : public object, public std::enable_shared_from_this<MessageCompleteParams> {
public:
    using std::enable_shared_from_this<MessageCompleteParams>::shared_from_this;
    std::shared_ptr<UUID> messageId;

    any status;

    string error;
};

class ExternalMessageParams : public object, public std::enable_shared_from_this<ExternalMessageParams> {
public:
    using std::enable_shared_from_this<ExternalMessageParams>::shared_from_this;
    string platform;

    string channelId;

    array<object> messages;
};

class ChannelCreateParams : public object, public std::enable_shared_from_this<ChannelCreateParams> {
public:
    using std::enable_shared_from_this<ChannelCreateParams>::shared_from_this;
    string name;

    std::shared_ptr<ChannelType> type;

    std::shared_ptr<UUID> serverId;

    Record<string, any> metadata;
};

class GroupChannelCreateParams : public object, public std::enable_shared_from_this<GroupChannelCreateParams> {
public:
    using std::enable_shared_from_this<GroupChannelCreateParams>::shared_from_this;
    string name;

    array<std::shared_ptr<UUID>> participantIds;

    Record<string, any> metadata;
};

class DmChannelParams : public object, public std::enable_shared_from_this<DmChannelParams> {
public:
    using std::enable_shared_from_this<DmChannelParams>::shared_from_this;
    std::tuple<std::shared_ptr<UUID>, std::shared_ptr<UUID>> participantIds;
};

class ChannelParticipant : public object, public std::enable_shared_from_this<ChannelParticipant> {
public:
    using std::enable_shared_from_this<ChannelParticipant>::shared_from_this;
    std::shared_ptr<UUID> id;

    std::shared_ptr<UUID> channelId;

    std::shared_ptr<UUID> userId;

    string role;

    std::shared_ptr<Date> joinedAt;
};

class MessageSearchParams : public PaginationParams, public std::enable_shared_from_this<MessageSearchParams> {
public:
    using std::enable_shared_from_this<MessageSearchParams>::shared_from_this;
    string query;

    std::shared_ptr<UUID> channelId;

    std::shared_ptr<UUID> authorId;

    any from;

    any to;
};

class ServerCreateParams : public object, public std::enable_shared_from_this<ServerCreateParams> {
public:
    using std::enable_shared_from_this<ServerCreateParams>::shared_from_this;
    string name;

    string sourceType;

    string sourceId;

    Record<string, any> metadata;
};

class ServerSyncParams : public object, public std::enable_shared_from_this<ServerSyncParams> {
public:
    using std::enable_shared_from_this<ServerSyncParams>::shared_from_this;
    array<object> channels;
};

class ChannelUpdateParams : public object, public std::enable_shared_from_this<ChannelUpdateParams> {
public:
    using std::enable_shared_from_this<ChannelUpdateParams>::shared_from_this;
    string name;

    array<std::shared_ptr<UUID>> participantCentralUserIds;

    Record<string, any> metadata;
};

#endif
