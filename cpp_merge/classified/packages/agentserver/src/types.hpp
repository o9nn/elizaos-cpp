#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_AGENTSERVER_SRC_TYPES_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_AGENTSERVER_SRC_TYPES_H
#include "core.hpp"
// External dependency removed

class MessageServer;
class MessageChannel;
class CentralRootMessage;
class MessageServiceStructure;
namespace @elizaos/core {
    class ServiceTypeRegistry;
}

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

    std::shared_ptr<UUID> serverId;

    string name;

    std::shared_ptr<ChannelType> type;

    string sourceType;

    string sourceId;

    string topic;

    Record<string, any> metadata;

    std::shared_ptr<Date> createdAt;

    std::shared_ptr<Date> updatedAt;
};

class CentralRootMessage : public object, public std::enable_shared_from_this<CentralRootMessage> {
public:
    using std::enable_shared_from_this<CentralRootMessage>::shared_from_this;
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

class MessageServiceStructure : public object, public std::enable_shared_from_this<MessageServiceStructure> {
public:
    using std::enable_shared_from_this<MessageServiceStructure>::shared_from_this;
    std::shared_ptr<UUID> id;

    std::shared_ptr<UUID> channel_id;

    std::shared_ptr<UUID> server_id;

    std::shared_ptr<UUID> author_id;

    string author_display_name;

    string content;

    any raw_message;

    string source_id;

    string source_type;

    std::shared_ptr<UUID> in_reply_to_message_id;

    double created_at;

    any metadata;
};

namespace std:("@elizaos/core") {
    class ServiceTypeRegistry : public object, public std::enable_shared_from_this<ServiceTypeRegistry> {
    public:
        using std::enable_shared_from_this<ServiceTypeRegistry>::shared_from_this;
        string MESSAGE_BUS_SERVICE;
    };

}
extern object AgentServerServiceType, satisfies, Partial;
namespace @elizaos/core {
}
#endif
