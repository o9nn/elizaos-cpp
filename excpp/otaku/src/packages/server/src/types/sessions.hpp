#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use


/**
 * Session timeout configuration
 */
struct SessionTimeoutConfig {
    std::optional<double> timeoutMinutes;
    std::optional<bool> autoRenew;
    std::optional<double> maxDurationMinutes;
    std::optional<double> warningThresholdMinutes;
};

/**
 * Metadata associated with a session
 */
struct SessionMetadata {
    std::optional<std::string> platform;
    std::optional<std::string> username;
    std::optional<std::string> discriminator;
    std::optional<std::string> avatar;
};

/**
 * Represents a messaging session between a user and an agent
 */
struct Session {
    std::string id;
    UUID agentId;
    UUID channelId;
    UUID userId;
    SessionMetadata metadata;
    Date createdAt;
    Date lastActivity;
    Date expiresAt;
    SessionTimeoutConfig timeoutConfig;
    double renewalCount;
    bool sent;
    std::optional<Date> sentAt;
};

/**
 * Request body for creating a session
 */
struct CreateSessionRequest {
    std::string agentId;
    std::string userId;
    std::optional<SessionMetadata> metadata;
    std::optional<SessionTimeoutConfig> timeoutConfig;
};

/**
 * Response for session creation
 */
struct CreateSessionResponse {
    std::string sessionId;
    UUID agentId;
    UUID userId;
    Date createdAt;
    SessionMetadata metadata;
    Date expiresAt;
    SessionTimeoutConfig timeoutConfig;
};

/**
 * Request body for sending a message
 */
struct SendMessageRequest {
    std::string content;
    std::string type;
    std::string url;
    std::optional<std::string> name;
    std::optional<MessageMetadata> metadata;
};

/**
 * Query parameters for retrieving messages
 */
struct GetMessagesQuery {
    std::optional<std::string> limit;
    std::optional<std::string> before;
    std::optional<std::string> after;
};

/**
 * Simplified message format for API responses
 */
struct SimplifiedMessage {
    std::string id;
    std::string content;
    std::string authorId;
    bool isAgent;
    Date createdAt;
    std::optional<std::string> thought;
    std::optional<std::vector<std::string>> actions;
};

/**
 * Response for message retrieval
 */
struct GetMessagesResponse {
    std::vector<SimplifiedMessage> messages;
    bool hasMore;
    std::optional<double> before;
    std::optional<double> after;
};

/**
 * Session info response
 */
struct SessionInfoResponse {
    std::string sessionId;
    UUID agentId;
    UUID userId;
    Date createdAt;
    Date lastActivity;
    SessionMetadata metadata;
    Date expiresAt;
    SessionTimeoutConfig timeoutConfig;
    double renewalCount;
    double timeRemaining;
    bool isNearExpiration;
};

/**
 * Health check response
 */
struct HealthCheckResponse {
    std::string status;
    double activeSessions;
    std::string timestamp;
};


} // namespace elizaos
