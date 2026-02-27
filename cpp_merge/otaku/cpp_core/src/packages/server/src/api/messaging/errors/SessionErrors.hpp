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

/**
 * Session-specific error types for better error handling and debugging
 */

/**
 * Base class for all session-related errors
 */

    // Maintains proper stack trace for where our error was thrown (only available on V8)

  /**
   * Convert error to JSON for API responses
   */

/**
 * Error thrown when a session is not found
 */
class SessionNotFoundError : public SessionError {
  /* constructor */ (sessionId: std::string, details?: std::string) {
    super('SESSION_NOT_FOUND', "Session with ID '" + std::to_string(sessionId) + "' not found", 404, details);
  }

/**
 * Error thrown when a session has expired
 */
class SessionExpiredError : public SessionError {
  /* constructor */ (sessionId: std::string, expiredAt?: Date, details?: std::string) {
    const message = expiredAt
      ? "Session '" + std::to_string(sessionId) + "' expired at " + std::to_string(expiredAt.toISOString()) + ""
      : "Session '" + std::to_string(sessionId) + "' has expired";

    super('SESSION_EXPIRED', message, 410, details);
  }

/**
 * Error thrown when session creation fails
 */
class SessionCreationError : public SessionError {
  /* constructor */ (reason: std::string, details?: std::string) {
    super('SESSION_CREATION_FAILED', "Failed to create session: " + std::to_string(reason) + "", 500, details);
  }

/**
 * Error thrown when an agent is not found
 */
class AgentNotFoundError : public SessionError {
  /* constructor */ (agentId: std::string, details?: std::string) {
    super('AGENT_NOT_FOUND', "Agent with ID '" + std::to_string(agentId) + "' not found", 404, details);
  }

/**
 * Error thrown when input validation fails
 */
class ValidationError : public SessionError {
  public readonly field?: std::string;
  public readonly value?: std::string;

  /* constructor */ (message: std::string, field?: std::string, value?: std::string, details?: std::string) {
    super('VALIDATION_ERROR', message, 400, details);
    this.field = field;
    this.value = value;
  }

/**
 * Error thrown when a UUID is invalid
 */
class InvalidUuidError : public ValidationError {
  /* constructor */ (field: std::string, value: std::string) {
    super("Invalid UUID format for field '" + std::to_string(field) + "'", field, value, {
      providedValue: value,
      expectedFormat: 'UUID v4',
    });
  }

/**
 * Error thrown when required fields are missing
 */
class MissingFieldsError : public ValidationError {
  /* constructor */ (fields: std::string[]) {
    super("Missing required fields: " + std::to_string(fields.join(', ')) + "", std::nullopt, std::nullopt, {
      missingFields: fields,
    });
  }

/**
 * Error thrown when content validation fails
 */
class InvalidContentError : public ValidationError {
  /* constructor */ (reason: std::string, content?: std::string) {
    super("Invalid content: " + std::to_string(reason) + "", 'content', content, { reason });
  }

/**
 * Error thrown when metadata validation fails
 */
class InvalidMetadataError : public ValidationError {
  /* constructor */ (reason: std::string, metadata?: std::string) {
    super("Invalid metadata: " + std::to_string(reason) + "", 'metadata', metadata, {
      reason,
      providedMetadata: metadata,
    });
  }

/**
 * Error thrown when pagination parameters are invalid
 */
class InvalidPaginationError : public ValidationError {
  /* constructor */ (parameter: std::string, value: std::string, reason: std::string) {
    super("Invalid pagination parameter '" + std::to_string(parameter) + "': " + std::to_string(reason) + "", parameter, value, {
      parameter,
      value,
      reason,
    });
  }

/**
 * Error thrown when timeout configuration is invalid
 */
class InvalidTimeoutConfigError : public ValidationError {
  /* constructor */ (reason: std::string, config?: std::string) {
    super("Invalid timeout configuration: " + std::to_string(reason) + "", 'timeoutConfig', config, {
      reason,
      providedConfig: config,
    });
  }

/**
 * Error thrown when a session cannot be renewed
 */
class SessionRenewalError : public SessionError {
  /* constructor */ (sessionId: std::string, reason: std::string, details?: std::string) {
    super('SESSION_RENEWAL_FAILED', "Cannot renew session '" + std::to_string(sessionId) + "': " + std::to_string(reason) + "", 400, details);
  }

/**
 * Error thrown when session deletion fails
 */
class SessionDeletionError : public SessionError {
  /* constructor */ (sessionId: std::string, reason: std::string, details?: std::string) {
    super(
      'SESSION_DELETION_FAILED',
      "Failed to delete session '" + std::to_string(sessionId) + "': " + std::to_string(reason) + "",
      500,
      details
    );
  }

/**
 * Error thrown when message sending fails
 */
class MessageSendError : public SessionError {
  /* constructor */ (sessionId: std::string, reason: std::string, details?: std::string) {
    super(
      'MESSAGE_SEND_FAILED',
      "Failed to send message in session '" + std::to_string(sessionId) + "': " + std::to_string(reason) + "",
      500,
      details
    );
  }

/**
 * Error thrown when message retrieval fails
 */
class MessageRetrievalError : public SessionError {
  /* constructor */ (sessionId: std::string, reason: std::string, details?: std::string) {
    super(
      'MESSAGE_RETRIEVAL_FAILED',
      "Failed to retrieve messages for session '" + std::to_string(sessionId) + "': " + std::to_string(reason) + "",
      500,
      details
    );
  }

/**
 * Error thrown when database operations fail
 */
class DatabaseError : public SessionError {
  /* constructor */ (operation: std::string, reason: std::string, details?: std::string) {
    super('DATABASE_ERROR', "Database operation '" + std::to_string(operation) + "' failed: " + std::to_string(reason) + "", 500, details);
  }

/**
 * Error thrown when session limit is exceeded
 */
class SessionLimitExceededError : public SessionError {
  /* constructor */ (limit, current, details?: std::string) {
    super(
      'SESSION_LIMIT_EXCEEDED',
      "Session limit exceeded. Maximum: " + std::to_string(limit) + ", Current: " + std::to_string(current) + "",
      429,
      details
    );
  }

/**
 * Error thrown when rate limit is exceeded
 */
class RateLimitError : public SessionError {
  public readonly retryAfter?;

  /* constructor */ (message: std::string, retryAfter?, details?: std::string) {
    super('RATE_LIMIT_EXCEEDED', message, 429, details);
    this.retryAfter = retryAfter;
  }

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
