#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_CORE_SRC_TYPES_MESSAGE_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_CORE_SRC_TYPES_MESSAGE_H
#include "core.hpp"
#include <string>
#include "./service.h"
#include "./primitives.h"

class MessageParticipant;
class MessageAttachment;
class MessageReaction;
class MessageReference;
class MessageContent;
class MessageInfo;
class MessageSendOptions;
class MessageSearchOptions;
class MessageChannel;
class IMessageService;

class MessageParticipant : public object, public std::enable_shared_from_this<MessageParticipant> {
public:
    using std::enable_shared_from_this<MessageParticipant>::shared_from_this;
    std::shared_ptr<UUID> id;

    string name;

    string username;

    string avatar;

    any status;
};

class MessageAttachment : public object, public std::enable_shared_from_this<MessageAttachment> {
public:
    using std::enable_shared_from_this<MessageAttachment>::shared_from_this;
    std::shared_ptr<UUID> id;

    string filename;

    string url;

    string mimeType;

    double size;

    double width;

    double height;

    double duration;

    string thumbnail;
};

class MessageReaction : public object, public std::enable_shared_from_this<MessageReaction> {
public:
    using std::enable_shared_from_this<MessageReaction>::shared_from_this;
    string emoji;

    double count;

    array<std::shared_ptr<UUID>> users;

    boolean hasReacted;
};

class MessageReference : public object, public std::enable_shared_from_this<MessageReference> {
public:
    using std::enable_shared_from_this<MessageReference>::shared_from_this;
    std::shared_ptr<UUID> messageId;

    std::shared_ptr<UUID> channelId;

    any type;
};

class MessageContent : public object, public std::enable_shared_from_this<MessageContent> {
public:
    using std::enable_shared_from_this<MessageContent>::shared_from_this;
    string text;

    string html;

    string markdown;

    array<std::shared_ptr<MessageAttachment>> attachments;

    array<std::shared_ptr<MessageReaction>> reactions;

    std::shared_ptr<MessageReference> reference;

    array<std::shared_ptr<UUID>> mentions;

    array<object> embeds;
};

class MessageInfo : public object, public std::enable_shared_from_this<MessageInfo> {
public:
    using std::enable_shared_from_this<MessageInfo>::shared_from_this;
    std::shared_ptr<UUID> id;

    std::shared_ptr<UUID> channelId;

    std::shared_ptr<UUID> senderId;

    std::shared_ptr<MessageContent> content;

    std::shared_ptr<Date> timestamp;

    std::shared_ptr<Date> edited;

    std::shared_ptr<Date> deleted;

    boolean pinned;

    object thread;
};

class MessageSendOptions : public object, public std::enable_shared_from_this<MessageSendOptions> {
public:
    using std::enable_shared_from_this<MessageSendOptions>::shared_from_this;
    std::shared_ptr<UUID> replyTo;

    boolean ephemeral;

    boolean silent;

    std::shared_ptr<Date> scheduled;

    std::shared_ptr<UUID> thread;

    string nonce;
};

class MessageSearchOptions : public object, public std::enable_shared_from_this<MessageSearchOptions> {
public:
    using std::enable_shared_from_this<MessageSearchOptions>::shared_from_this;
    string query;

    std::shared_ptr<UUID> channelId;

    std::shared_ptr<UUID> senderId;

    std::shared_ptr<Date> before;

    std::shared_ptr<Date> after;

    double limit;

    double offset;

    boolean hasAttachments;

    boolean pinned;

    std::shared_ptr<UUID> mentions;
};

class MessageChannel : public object, public std::enable_shared_from_this<MessageChannel> {
public:
    using std::enable_shared_from_this<MessageChannel>::shared_from_this;
    std::shared_ptr<UUID> id;

    string name;

    any type;

    string description;

    array<std::shared_ptr<MessageParticipant>> participants;

    object permissions;

    std::shared_ptr<Date> lastMessageAt;

    double messageCount;

    double unreadCount;
};

class IMessageService : public Service, public std::enable_shared_from_this<IMessageService> {
public:
    using std::enable_shared_from_this<IMessageService>::shared_from_this;
    static any override;

    string serviceType = ServiceType["MESSAGE"];

    string capabilityDescription = std::string("Message sending, receiving, and management capabilities");

    virtual std::shared_ptr<Promise<std::shared_ptr<UUID>>> sendMessage(std::shared_ptr<UUID> channelId, std::shared_ptr<MessageContent> content, std::shared_ptr<MessageSendOptions> options = std::nullopt) = 0;
    virtual std::shared_ptr<Promise<array<std::shared_ptr<MessageInfo>>>> getMessages(std::shared_ptr<UUID> channelId, std::shared_ptr<MessageSearchOptions> options = std::nullopt) = 0;
    virtual std::shared_ptr<Promise<std::shared_ptr<MessageInfo>>> getMessage(std::shared_ptr<UUID> messageId) = 0;
    virtual std::shared_ptr<Promise<void>> editMessage(std::shared_ptr<UUID> messageId, std::shared_ptr<MessageContent> content) = 0;
    virtual std::shared_ptr<Promise<void>> deleteMessage(std::shared_ptr<UUID> messageId) = 0;
    virtual std::shared_ptr<Promise<void>> addReaction(std::shared_ptr<UUID> messageId, string emoji) = 0;
    virtual std::shared_ptr<Promise<void>> removeReaction(std::shared_ptr<UUID> messageId, string emoji) = 0;
    virtual std::shared_ptr<Promise<void>> pinMessage(std::shared_ptr<UUID> messageId) = 0;
    virtual std::shared_ptr<Promise<void>> unpinMessage(std::shared_ptr<UUID> messageId) = 0;
    virtual std::shared_ptr<Promise<array<std::shared_ptr<MessageChannel>>>> getChannels() = 0;
    virtual std::shared_ptr<Promise<std::shared_ptr<MessageChannel>>> getChannel(std::shared_ptr<UUID> channelId) = 0;
    virtual std::shared_ptr<Promise<std::shared_ptr<UUID>>> createChannel(string name, any type, object options = std::nullopt) = 0;
    virtual std::shared_ptr<Promise<array<std::shared_ptr<MessageInfo>>>> searchMessages(string query, std::shared_ptr<MessageSearchOptions> options = std::nullopt) = 0;
    IMessageService(std::shared_ptr<IAgentRuntime> runtime = std::nullopt);
};

#endif
