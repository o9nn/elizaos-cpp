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
    std::string type;
    std::string url;
    std::optional<std::string> name;
    std::optional<double> size;
    std::optional<std::string> mimeType;
};

/**
 * Session message metadata type
 */
struct SessionMessageMetadata {
    std::optional<std::string> source;
    std::optional<std::string> priority;
    std::optional<std::vector<std::string>> tags;
    std::optional<std::string> thought;
    std::optional<std::vector<std::string>> actions;
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
    std::chrono::system_clock::time_point createdAt;
    std::chrono::system_clock::time_point lastActivity;
};

/**
 * Request parameters for creating a session
 */
struct CreateSessionParams {
    std::string agentId;
    std::string userId;
    std::optional<SessionMetadata> metadata;
};

/**
 * Response for session creation
 */
struct CreateSessionResponse {
    std::string sessionId;
    UUID agentId;
    UUID userId;
    std::chrono::system_clock::time_point createdAt;
    SessionMetadata metadata;
};

/**
 * Request parameters for sending a message
 */
struct SendMessageParams {
    std::string content;
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
    std::string id;
    std::string content;
    std::string authorId;
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
    std::string sessionId;
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
    std::string status;
    double activeSessions;
    std::string timestamp;
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
    std::string id;
    std::string content;
    std::string authorId;
    std::chrono::system_clock::time_point createdAt;
    std::optional<SessionMessageMetadata> metadata;
};


} // namespace elizaos
