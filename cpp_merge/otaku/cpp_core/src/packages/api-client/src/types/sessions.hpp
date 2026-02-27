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
 * Attachment type for messages
 */
struct MessageAttachment {
    std: type;
    std: url;
    std::optional<std:> name;
    std::optional<double> size;
    std::optional<std:> mimeType;
};

/**
 * Session message metadata type
 */
struct SessionMessageMetadata {
    std::optional<std:> source;
    std::optional<std:> priority;
    std::optional<std::vector<std::string>> tags;
    std::optional<std:> thought;
    std::optional<std::vector<std::string>> actions;
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
};

/**
 * Request parameters for creating a session
 */
struct CreateSessionParams {
    std: agentId;
    std: userId;
    std::optional<SessionMetadata> metadata;
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
};

/**
 * Request parameters for sending a message
 */
struct SendMessageParams {
    std: content;
    std::optional<std::vector<MessageAttachment>> attachments;
    std::optional<SessionMessageMetadata> metadata;
};

/**
 * Query parameters for retrieving messages
 */

/**
 * Simplified message format for API responses
 */
struct SimplifiedMessage {
    std: id;
    std: content;
    std: authorId;
    bool isAgent;
    std::chrono::system_clock::time_point createdAt;
    SessionMessageMetadata metadata;
};

/**
 * Response for message retrieval
 */
struct GetMessagesResponse {
    std::vector<SimplifiedMessage> messages;
    bool hasMore;
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
};

/**
 * Health check response
 */
struct SessionsHealthResponse {
    std: status;
    double activeSessions;
    std: timestamp;
};

/**
 * List sessions response
 */
struct ListSessionsResponse {
    std::vector<SessionInfoResponse> sessions;
    double total;
};

/**
 * Message response when sending a message
 */
struct MessageResponse {
    std: id;
    std: content;
    std: authorId;
    std::chrono::system_clock::time_point createdAt;
    std::optional<SessionMessageMetadata> metadata;
};


} // namespace elizaos
