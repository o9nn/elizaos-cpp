#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_OTAKU_SRC_PACKAGES_SERVER_SRC_TYPES_SESSIONS_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_OTAKU_SRC_PACKAGES_SERVER_SRC_TYPES_SESSIONS_H
#include "core.hpp"
// External dependency removed
// External dependency removed

class SessionTimeoutConfig;
class SessionMetadata;
class Session;
class CreateSessionRequest;
class CreateSessionResponse;
class SendMessageRequest;
class GetMessagesQuery;
class SimplifiedMessage;
class GetMessagesResponse;
class SessionInfoResponse;
class HealthCheckResponse;

class SessionTimeoutConfig : public object, public std::enable_shared_from_this<SessionTimeoutConfig> {
public:
    using std::enable_shared_from_this<SessionTimeoutConfig>::shared_from_this;
    double timeoutMinutes;

    boolean autoRenew;

    double maxDurationMinutes;

    double warningThresholdMinutes;
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

    std::shared_ptr<Date> expiresAt;

    std::shared_ptr<SessionTimeoutConfig> timeoutConfig;

    double renewalCount;

    object warningState;
};

class CreateSessionRequest : public object, public std::enable_shared_from_this<CreateSessionRequest> {
public:
    using std::enable_shared_from_this<CreateSessionRequest>::shared_from_this;
    string agentId;

    string userId;

    std::shared_ptr<SessionMetadata> metadata;

    std::shared_ptr<SessionTimeoutConfig> timeoutConfig;
};

class CreateSessionResponse : public object, public std::enable_shared_from_this<CreateSessionResponse> {
public:
    using std::enable_shared_from_this<CreateSessionResponse>::shared_from_this;
    string sessionId;

    std::shared_ptr<UUID> agentId;

    std::shared_ptr<UUID> userId;

    std::shared_ptr<Date> createdAt;

    std::shared_ptr<SessionMetadata> metadata;

    std::shared_ptr<Date> expiresAt;

    std::shared_ptr<SessionTimeoutConfig> timeoutConfig;
};

class SendMessageRequest : public object, public std::enable_shared_from_this<SendMessageRequest> {
public:
    using std::enable_shared_from_this<SendMessageRequest>::shared_from_this;
    string content;

    array<object> attachments;

    std::shared_ptr<MessageMetadata> metadata;
};

class GetMessagesQuery : public object, public std::enable_shared_from_this<GetMessagesQuery> {
public:
    using std::enable_shared_from_this<GetMessagesQuery>::shared_from_this;
    string limit;

    string before;

    string after;
};

class SimplifiedMessage : public object, public std::enable_shared_from_this<SimplifiedMessage> {
public:
    using std::enable_shared_from_this<SimplifiedMessage>::shared_from_this;
    string id;

    string content;

    string authorId;

    boolean isAgent;

    std::shared_ptr<Date> createdAt;

    object metadata;
};

class GetMessagesResponse : public object, public std::enable_shared_from_this<GetMessagesResponse> {
public:
    using std::enable_shared_from_this<GetMessagesResponse>::shared_from_this;
    array<std::shared_ptr<SimplifiedMessage>> messages;

    boolean hasMore;

    object cursors;
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

    std::shared_ptr<Date> expiresAt;

    std::shared_ptr<SessionTimeoutConfig> timeoutConfig;

    double renewalCount;

    double timeRemaining;

    boolean isNearExpiration;
};

class HealthCheckResponse : public object, public std::enable_shared_from_this<HealthCheckResponse> {
public:
    using std::enable_shared_from_this<HealthCheckResponse>::shared_from_this;
    any status;

    double activeSessions;

    string timestamp;
};

#endif
