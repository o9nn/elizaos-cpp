#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CLIENT_SRC_TYPES_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CLIENT_SRC_TYPES_H
#include "core.h"
#include "@elizaos/core.h"
#include "@elizaos/core.h"
using CoreAgent = Agent;
using CoreCharacter = Character;
using CoreRoom = Room;
using CoreAgentStatus = AgentStatus;
using CoreChannelType = ChannelType;

class IAttachment;
class AgentWithStatus;
class AgentPanel;
class MessageServer;
class MessageChannel;
class ServerMessage;

class IAttachment : public object, public std::enable_shared_from_this<IAttachment> {
public:
    using std::enable_shared_from_this<IAttachment>::shared_from_this;
    string url;

    string contentType;

    string title;
};

class AgentWithStatus : public Partial<CoreAgent>, public std::enable_shared_from_this<AgentWithStatus> {
public:
    using std::enable_shared_from_this<AgentWithStatus>::shared_from_this;
    std::shared_ptr<UUID> id;

    string name;

    string characterName;

    any bio;

    std::shared_ptr<CoreAgentStatus> status;

    any settings;
};

class AgentPanel : public object, public std::enable_shared_from_this<AgentPanel> {
public:
    using std::enable_shared_from_this<AgentPanel>::shared_from_this;
    string name;

    string path;
};

class MessageServer : public object, public std::enable_shared_from_this<MessageServer> {
public:
    using std::enable_shared_from_this<MessageServer>::shared_from_this;
    std::shared_ptr<UUID> id;

    string name;

    string sourceType;

    string sourceId;

    Record<string, any> metadata;

    string createdAt;

    string updatedAt;
};

class MessageChannel : public object, public std::enable_shared_from_this<MessageChannel> {
public:
    using std::enable_shared_from_this<MessageChannel>::shared_from_this;
    std::shared_ptr<UUID> id;

    std::shared_ptr<UUID> messageServerId;

    string name;

    std::shared_ptr<CoreChannelType> type;

    string sourceType;

    string sourceId;

    string topic;

    Record<string, any> metadata;

    string createdAt;

    string updatedAt;
};

class ServerMessage : public object, public std::enable_shared_from_this<ServerMessage> {
public:
    using std::enable_shared_from_this<ServerMessage>::shared_from_this;
    std::shared_ptr<UUID> id;

    std::shared_ptr<UUID> channelId;

    std::shared_ptr<UUID> serverId;

    std::shared_ptr<UUID> authorId;

    string authorDisplayName;

    string content;

    double createdAt;

    any rawMessage;

    std::shared_ptr<UUID> inReplyToRootMessageId;

    string sourceType;

    string sourceId;

    object metadata;
};

#endif
