#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTAKU_SRC_PACKAGES_SERVER_SRC_TYPES_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTAKU_SRC_PACKAGES_SERVER_SRC_TYPES_H
#include "core.h"
#include "@elizaos/core.h"
#include "@elizaos/api-client.h"

typedef any AttachmentInput;

class MessageServer;
class MessageChannel;
class CentralRootMessage;
class MessageServiceStructure;
class Attachment;
class MessageContentWithAttachments;
class MessageMetadataWithAttachments;
class MessageWithAttachments;

class MessageServer : public object, public std::enable_shared_from_this<MessageServer> {
public:
    using std::enable_shared_from_this<MessageServer>::shared_from_this;
    std::shared_ptr<UUID> id;

    string name;

    string sourceType;

    string sourceId;

    std::shared_ptr<ServerMetadata> metadata;

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

    std::shared_ptr<ChannelMetadata> metadata;

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

    std::shared_ptr<MessageMetadata> metadata;
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

    std::shared_ptr<MessageMetadata> metadata;
};

class Attachment : public object, public std::enable_shared_from_this<Attachment> {
public:
    using std::enable_shared_from_this<Attachment>::shared_from_this;
    string url;
};

class MessageContentWithAttachments : public object, public std::enable_shared_from_this<MessageContentWithAttachments> {
public:
    using std::enable_shared_from_this<MessageContentWithAttachments>::shared_from_this;
    AttachmentInput attachments;
};

class MessageMetadataWithAttachments : public object, public std::enable_shared_from_this<MessageMetadataWithAttachments> {
public:
    using std::enable_shared_from_this<MessageMetadataWithAttachments>::shared_from_this;
    AttachmentInput attachments;
};

class MessageWithAttachments : public object, public std::enable_shared_from_this<MessageWithAttachments> {
public:
    using std::enable_shared_from_this<MessageWithAttachments>::shared_from_this;
    any content;

    std::shared_ptr<MessageMetadataWithAttachments> metadata;
};

#endif
