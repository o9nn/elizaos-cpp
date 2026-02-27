#pragma once
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
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
    std::optional<std:> platform;
    std::optional<std:> username;
    std::optional<std:> discriminator;
    std::optional<std:> avatar;
};

/**
 * Represents a messaging session between a user and an agent
 */
struct Session {
    std: id;
    UUID agentId;
    UUID channelId;
    UUID userId;
    SessionMetadata metadata;
    std::chrono::system_clock::time_point createdAt;
    std::chrono::system_clock::time_point lastActivity;
    std::chrono::system_clock::time_point expiresAt;
    SessionTimeoutConfig timeoutConfig;
    double renewalCount;
    bool sent;
    std::optional<std::chrono::system_clock::time_point> sentAt;
};

/**
 * Request body for creating a session
 */
struct CreateSessionRequest {
    std: agentId;
    std: userId;
    std::optional<SessionMetadata> metadata;
    std::optional<SessionTimeoutConfig> timeoutConfig;
};

/**
 * Response for session creation
 */
struct CreateSessionResponse {
    std: sessionId;
    UUID agentId;
    UUID userId;
    std::chrono::system_clock::time_point createdAt;
    SessionMetadata metadata;
    std::chrono::system_clock::time_point expiresAt;
    SessionTimeoutConfig timeoutConfig;
};

/**
 * Request body for sending a message
 */
struct SendMessageRequest {
    std: content;
    std: type;
    std: url;
    std::optional<std:> name;
    std::optional<MessageMetadata> metadata;
};

/**
 * Query parameters for retrieving messages
 */
struct GetMessagesQuery {
    std::optional<std:> limit;
    std::optional<std:> before;
    std::optional<std:> after;
};

/**
 * Simplified message format for API responses
 */
struct SimplifiedMessage {
    std: id;
    std: content;
    std: authorId;
    bool isAgent;
    std::chrono::system_clock::time_point createdAt;
    std::optional<std:> thought;
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
    std: sessionId;
    UUID agentId;
    UUID userId;
    std::chrono::system_clock::time_point createdAt;
    std::chrono::system_clock::time_point lastActivity;
    SessionMetadata metadata;
    std::chrono::system_clock::time_point expiresAt;
    SessionTimeoutConfig timeoutConfig;
    double renewalCount;
    double timeRemaining;
    bool isNearExpiration;
};

/**
 * Health check response
 */
struct HealthCheckResponse {
    std: status;
    double activeSessions;
    std: timestamp;
};


} // namespace elizaos
