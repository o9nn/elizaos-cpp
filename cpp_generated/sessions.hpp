#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTAKU_SRC_PACKAGES_API-CLIENT_SRC_TYPES_SESSIONS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTAKU_SRC_PACKAGES_API-CLIENT_SRC_TYPES_SESSIONS_H
#include "core.h"
#include "@elizaos/core.h"
#include "./base.h"

class MessageAttachment;
class SessionMessageMetadata;
class SessionMetadata;
class Session;
class CreateSessionParams;
class CreateSessionResponse;
class SendMessageParams;
class GetMessagesParams;
class SimplifiedMessage;
class GetMessagesResponse;
class SessionInfoResponse;
class SessionsHealthResponse;
class ListSessionsResponse;
class MessageResponse;

class MessageAttachment : public object, public std::enable_shared_from_this<MessageAttachment> {
public:
    using std::enable_shared_from_this<MessageAttachment>::shared_from_this;
    any type;

    string url;

    string name;

    double size;

    string mimeType;
};

class SessionMessageMetadata : public object, public std::enable_shared_from_this<SessionMessageMetadata> {
public:
    using std::enable_shared_from_this<SessionMessageMetadata>::shared_from_this;
    string source;

    any priority;

    array<string> tags;

    Record<string, any> context;

    string thought;

    array<string> actions;
};

class SessionMetadata : public object, public std::enable_shared_from_this<SessionMetadata> {
public:
    using std::enable_shared_from_this<SessionMetadata>::shared_from_this;
    string platform;

    string username;

    string discriminator;

    string avatar;
};

class Session : public object, public std::enable_shared_from_this<Session> {
public:
    using std::enable_shared_from_this<Session>::shared_from_this;
    string id;

    std::shared_ptr<UUID> agentId;

    std::shared_ptr<UUID> channelId;

    std::shared_ptr<UUID> userId;

    std::shared_ptr<SessionMetadata> metadata;

    std::shared_ptr<Date> createdAt;

    std::shared_ptr<Date> lastActivity;
};

class CreateSessionParams : public object, public std::enable_shared_from_this<CreateSessionParams> {
public:
    using std::enable_shared_from_this<CreateSessionParams>::shared_from_this;
    string agentId;

    string userId;

    std::shared_ptr<SessionMetadata> metadata;
};

class CreateSessionResponse : public object, public std::enable_shared_from_this<CreateSessionResponse> {
public:
    using std::enable_shared_from_this<CreateSessionResponse>::shared_from_this;
    string sessionId;

    std::shared_ptr<UUID> agentId;

    std::shared_ptr<UUID> userId;

    std::shared_ptr<Date> createdAt;

    std::shared_ptr<SessionMetadata> metadata;
};

class SendMessageParams : public object, public std::enable_shared_from_this<SendMessageParams> {
public:
    using std::enable_shared_from_this<SendMessageParams>::shared_from_this;
    string content;

    array<std::shared_ptr<MessageAttachment>> attachments;

    std::shared_ptr<SessionMessageMetadata> metadata;
};

class GetMessagesParams : public PaginationParams, public std::enable_shared_from_this<GetMessagesParams> {
public:
    using std::enable_shared_from_this<GetMessagesParams>::shared_from_this;
    any before;

    any after;
};

class SimplifiedMessage : public object, public std::enable_shared_from_this<SimplifiedMessage> {
public:
    using std::enable_shared_from_this<SimplifiedMessage>::shared_from_this;
    string id;

    string content;

    string authorId;

    boolean isAgent;

    std::shared_ptr<Date> createdAt;

    std::shared_ptr<SessionMessageMetadata> metadata;
};

class GetMessagesResponse : public object, public std::enable_shared_from_this<GetMessagesResponse> {
public:
    using std::enable_shared_from_this<GetMessagesResponse>::shared_from_this;
    array<std::shared_ptr<SimplifiedMessage>> messages;

    boolean hasMore;
};

class SessionInfoResponse : public object, public std::enable_shared_from_this<SessionInfoResponse> {
public:
    using std::enable_shared_from_this<SessionInfoResponse>::shared_from_this;
    string sessionId;

    std::shared_ptr<UUID> agentId;

    std::shared_ptr<UUID> userId;

    std::shared_ptr<Date> createdAt;

    std::shared_ptr<Date> lastActivity;

    std::shared_ptr<SessionMetadata> metadata;
};

class SessionsHealthResponse : public object, public std::enable_shared_from_this<SessionsHealthResponse> {
public:
    using std::enable_shared_from_this<SessionsHealthResponse>::shared_from_this;
    any status;

    double activeSessions;

    string timestamp;
};

class ListSessionsResponse : public object, public std::enable_shared_from_this<ListSessionsResponse> {
public:
    using std::enable_shared_from_this<ListSessionsResponse>::shared_from_this;
    array<std::shared_ptr<SessionInfoResponse>> sessions;

    double total;
};

class MessageResponse : public object, public std::enable_shared_from_this<MessageResponse> {
public:
    using std::enable_shared_from_this<MessageResponse>::shared_from_this;
    string id;

    string content;

    string authorId;

    std::shared_ptr<Date> createdAt;

    std::shared_ptr<SessionMessageMetadata> metadata;
};

#endif
