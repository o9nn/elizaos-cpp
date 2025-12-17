#pragma once
#include <any>
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
 * Session-specific error types for better error handling and debugging
 */

/**
 * Base class for all session-related errors
 */
    void toJSON();

/**
 * Error thrown when a session is not found
 */
class SessionNotFoundError {
public:
    SessionNotFoundError(const std::string& sessionId, std::optional<std::any> details);
};

/**
 * Error thrown when a session has expired
 */
class SessionExpiredError {
public:
    SessionExpiredError(const std::string& sessionId, std::optional<Date> expiredAt, std::optional<std::any> details);
};

/**
 * Error thrown when session creation fails
 */
class SessionCreationError {
public:
    SessionCreationError(const std::string& reason, std::optional<std::any> details);
};

/**
 * Error thrown when an agent is not found
 */
class AgentNotFoundError {
public:
    AgentNotFoundError(const std::string& agentId, std::optional<std::any> details);
};

/**
 * Error thrown when input validation fails
 */
class ValidationError {
public:
    ValidationError(const std::string& message, std::optional<std::string> field, std::optional<std::any> value, std::optional<std::any> details);
};

/**
 * Error thrown when a UUID is invalid
 */
class InvalidUuidError {
public:
    InvalidUuidError(const std::string& field, const std::string& value);
};

/**
 * Error thrown when required fields are missing
 */
class MissingFieldsError {
public:
    MissingFieldsError(const std::vector<std::string>& fields);
};

/**
 * Error thrown when content validation fails
 */
class InvalidContentError {
public:
    InvalidContentError(const std::string& reason, std::optional<std::any> content);
};

/**
 * Error thrown when metadata validation fails
 */
class InvalidMetadataError {
public:
    InvalidMetadataError(const std::string& reason, std::optional<std::any> metadata);
};

/**
 * Error thrown when pagination parameters are invalid
 */
class InvalidPaginationError {
public:
    InvalidPaginationError(const std::string& parameter, const std::any& value, const std::string& reason);
};

/**
 * Error thrown when timeout configuration is invalid
 */
class InvalidTimeoutConfigError {
public:
    InvalidTimeoutConfigError(const std::string& reason, std::optional<std::any> config);
};

/**
 * Error thrown when a session cannot be renewed
 */
class SessionRenewalError {
public:
    SessionRenewalError(const std::string& sessionId, const std::string& reason, std::optional<std::any> details);
};

/**
 * Error thrown when session deletion fails
 */
class SessionDeletionError {
public:
    SessionDeletionError(const std::string& sessionId, const std::string& reason, std::optional<std::any> details);
};

/**
 * Error thrown when message sending fails
 */
class MessageSendError {
public:
    MessageSendError(const std::string& sessionId, const std::string& reason, std::optional<std::any> details);
};

/**
 * Error thrown when message retrieval fails
 */
class MessageRetrievalError {
public:
    MessageRetrievalError(const std::string& sessionId, const std::string& reason, std::optional<std::any> details);
};

/**
 * Error thrown when database operations fail
 */
class DatabaseError {
public:
    DatabaseError(const std::string& operation, const std::string& reason, std::optional<std::any> details);
};

/**
 * Error thrown when session limit is exceeded
 */
class SessionLimitExceededError {
public:
    SessionLimitExceededError(double limit, double current, std::optional<std::any> details);
};

/**
 * Error thrown when rate limit is exceeded
 */
class RateLimitError {
public:
    RateLimitError(const std::string& message, std::optional<double> retryAfter, std::optional<std::any> details);
};

/**
 * Error handler utility for Express middleware
 */
void createErrorHandler();

/**
 * Type guard to check if an error is a SessionError
 */

/**
 * Type guard to check if an error is a validation error
 */

} // namespace elizaos
