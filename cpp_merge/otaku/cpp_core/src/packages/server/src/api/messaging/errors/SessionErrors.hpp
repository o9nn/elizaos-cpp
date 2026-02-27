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
class SessionNotFoundError extends SessionError {
  constructor(sessionId: std:, details?: std:) {
    super('SESSION_NOT_FOUND', "Session with ID '" + std::to_string(sessionId) + "' not found", 404, details);
  }

/**
 * Error thrown when a session has expired
 */
class SessionExpiredError extends SessionError {
  constructor(sessionId: std:, expiredAt?: Date, details?: std:) {
    const message = expiredAt
      ? "Session '" + std::to_string(sessionId) + "' expired at " + std::to_string(expiredAt.toISOString()) + ""
      : "Session '" + std::to_string(sessionId) + "' has expired";

    super('SESSION_EXPIRED', message, 410, details);
  }

/**
 * Error thrown when session creation fails
 */
class SessionCreationError extends SessionError {
  constructor(reason: std:, details?: std:) {
    super('SESSION_CREATION_FAILED', "Failed to create session: " + std::to_string(reason) + "", 500, details);
  }

/**
 * Error thrown when an agent is not found
 */
class AgentNotFoundError extends SessionError {
  constructor(agentId: std:, details?: std:) {
    super('AGENT_NOT_FOUND', "Agent with ID '" + std::to_string(agentId) + "' not found", 404, details);
  }

/**
 * Error thrown when input validation fails
 */
class ValidationError extends SessionError {
  public readonly field?: std:;
  public readonly value?: std:;

  constructor(message: std:, field?: std:, value?: std:, details?: std:) {
    super('VALIDATION_ERROR', message, 400, details);
    this.field = field;
    this.value = value;
  }

/**
 * Error thrown when a UUID is invalid
 */
class InvalidUuidError extends ValidationError {
  constructor(field: std:, value: std:) {
    super("Invalid UUID format for field '" + std::to_string(field) + "'", field, value, {
      providedValue: value,
      expectedFormat: 'UUID v4',
    });
  }

/**
 * Error thrown when required fields are missing
 */
class MissingFieldsError extends ValidationError {
  constructor(fields: std:[]) {
    super("Missing required fields: " + std::to_string(fields.join(', ')) + "", undefined, undefined, {
      missingFields: fields,
    });
  }

/**
 * Error thrown when content validation fails
 */
class InvalidContentError extends ValidationError {
  constructor(reason: std:, content?: std:) {
    super("Invalid content: " + std::to_string(reason) + "", 'content', content, { reason });
  }

/**
 * Error thrown when metadata validation fails
 */
class InvalidMetadataError extends ValidationError {
  constructor(reason: std:, metadata?: std:) {
    super("Invalid metadata: " + std::to_string(reason) + "", 'metadata', metadata, {
      reason,
      providedMetadata: metadata,
    });
  }

/**
 * Error thrown when pagination parameters are invalid
 */
class InvalidPaginationError extends ValidationError {
  constructor(parameter: std:, value: std:, reason: std:) {
    super("Invalid pagination parameter '" + std::to_string(parameter) + "': " + std::to_string(reason) + "", parameter, value, {
      parameter,
      value,
      reason,
    });
  }

/**
 * Error thrown when timeout configuration is invalid
 */
class InvalidTimeoutConfigError extends ValidationError {
  constructor(reason: std:, config?: std:) {
    super("Invalid timeout configuration: " + std::to_string(reason) + "", 'timeoutConfig', config, {
      reason,
      providedConfig: config,
    });
  }

/**
 * Error thrown when a session cannot be renewed
 */
class SessionRenewalError extends SessionError {
  constructor(sessionId: std:, reason: std:, details?: std:) {
    super('SESSION_RENEWAL_FAILED', "Cannot renew session '" + std::to_string(sessionId) + "': " + std::to_string(reason) + "", 400, details);
  }

/**
 * Error thrown when session deletion fails
 */
class SessionDeletionError extends SessionError {
  constructor(sessionId: std:, reason: std:, details?: std:) {
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
class MessageSendError extends SessionError {
  constructor(sessionId: std:, reason: std:, details?: std:) {
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
class MessageRetrievalError extends SessionError {
  constructor(sessionId: std:, reason: std:, details?: std:) {
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
class DatabaseError extends SessionError {
  constructor(operation: std:, reason: std:, details?: std:) {
    super('DATABASE_ERROR', "Database operation '" + std::to_string(operation) + "' failed: " + std::to_string(reason) + "", 500, details);
  }

/**
 * Error thrown when session limit is exceeded
 */
class SessionLimitExceededError extends SessionError {
  constructor(limit, current, details?: std:) {
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
class RateLimitError extends SessionError {
  public readonly retryAfter?;

  constructor(message: std:, retryAfter?, details?: std:) {
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
