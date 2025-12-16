#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

import type { UUID } from '@elizaos/core';
import type { PaginationParams } from './base';

/**
 * Attachment type for messages
 */
struct MessageAttachment {
    'image' | 'file' | 'audio' | 'video' | 'document' type;
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
    std::optional<'low' | 'normal' | 'high'> priority;
    std::optional<std::vector<std::string>> tags;
    std::optional<std::unordered_map<std::string, string | number | boolean>> context;
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
    Date createdAt;
    Date lastActivity;
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
    Date createdAt;
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
interface GetMessagesParams extends PaginationParams {
  before?: Date | string | number;
  after?: Date | string | number;
}

/**
 * Simplified message format for API responses
 */
struct SimplifiedMessage {
    std::string id;
    std::string content;
    std::string authorId;
    bool isAgent;
    Date createdAt;
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
    Date createdAt;
    Date lastActivity;
    SessionMetadata metadata;
};


/**
 * Health check response
 */
struct SessionsHealthResponse {
    'healthy' | 'unhealthy' status;
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
    Date createdAt;
    std::optional<SessionMessageMetadata> metadata;
};


} // namespace elizaos
