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
  constructor(sessionId: string, details?: any) {
    super('SESSION_NOT_FOUND', `Session with ID '${sessionId}' not found`, 404, details);
  }

/**
 * Error thrown when a session has expired
 */
class SessionExpiredError extends SessionError {
  constructor(sessionId: string, expiredAt?: Date, details?: any) {
    const message = expiredAt
      ? `Session '${sessionId}' expired at ${expiredAt.toISOString()}`
      : `Session '${sessionId}' has expired`;

    super('SESSION_EXPIRED', message, 410, details);
  }

/**
 * Error thrown when session creation fails
 */
class SessionCreationError extends SessionError {
  constructor(reason: string, details?: any) {
    super('SESSION_CREATION_FAILED', `Failed to create session: ${reason}`, 500, details);
  }

/**
 * Error thrown when an agent is not found
 */
class AgentNotFoundError extends SessionError {
  constructor(agentId: string, details?: any) {
    super('AGENT_NOT_FOUND', `Agent with ID '${agentId}' not found`, 404, details);
  }

/**
 * Error thrown when input validation fails
 */
class ValidationError extends SessionError {
  public readonly field?: string;
  public readonly value?: any;

  constructor(message: string, field?: string, value?: any, details?: any) {
    super('VALIDATION_ERROR', message, 400, details);
    this.field = field;
    this.value = value;
  }

/**
 * Error thrown when a UUID is invalid
 */
class InvalidUuidError extends ValidationError {
  constructor(field: string, value: string) {
    super(`Invalid UUID format for field '${field}'`, field, value, {
      providedValue: value,
      expectedFormat: 'UUID v4',
    });
  }

/**
 * Error thrown when required fields are missing
 */
class MissingFieldsError extends ValidationError {
  constructor(fields: string[]) {
    super(`Missing required fields: ${fields.join(', ')}`, undefined, undefined, {
      missingFields: fields,
    });
  }

/**
 * Error thrown when content validation fails
 */
class InvalidContentError extends ValidationError {
  constructor(reason: string, content?: any) {
    super(`Invalid content: ${reason}`, 'content', content, { reason });
  }

/**
 * Error thrown when metadata validation fails
 */
class InvalidMetadataError extends ValidationError {
  constructor(reason: string, metadata?: any) {
    super(`Invalid metadata: ${reason}`, 'metadata', metadata, {
      reason,
      providedMetadata: metadata,
    });
  }

/**
 * Error thrown when pagination parameters are invalid
 */
class InvalidPaginationError extends ValidationError {
  constructor(parameter: string, value: any, reason: string) {
    super(`Invalid pagination parameter '${parameter}': ${reason}`, parameter, value, {
      parameter,
      value,
      reason,
    });
  }

/**
 * Error thrown when timeout configuration is invalid
 */
class InvalidTimeoutConfigError extends ValidationError {
  constructor(reason: string, config?: any) {
    super(`Invalid timeout configuration: ${reason}`, 'timeoutConfig', config, {
      reason,
      providedConfig: config,
    });
  }

/**
 * Error thrown when a session cannot be renewed
 */
class SessionRenewalError extends SessionError {
  constructor(sessionId: string, reason: string, details?: any) {
    super('SESSION_RENEWAL_FAILED', `Cannot renew session '${sessionId}': ${reason}`, 400, details);
  }

/**
 * Error thrown when session deletion fails
 */
class SessionDeletionError extends SessionError {
  constructor(sessionId: string, reason: string, details?: any) {
    super(
      'SESSION_DELETION_FAILED',
      `Failed to delete session '${sessionId}': ${reason}`,
      500,
      details
    );
  }

/**
 * Error thrown when message sending fails
 */
class MessageSendError extends SessionError {
  constructor(sessionId: string, reason: string, details?: any) {
    super(
      'MESSAGE_SEND_FAILED',
      `Failed to send message in session '${sessionId}': ${reason}`,
      500,
      details
    );
  }

/**
 * Error thrown when message retrieval fails
 */
class MessageRetrievalError extends SessionError {
  constructor(sessionId: string, reason: string, details?: any) {
    super(
      'MESSAGE_RETRIEVAL_FAILED',
      `Failed to retrieve messages for session '${sessionId}': ${reason}`,
      500,
      details
    );
  }

/**
 * Error thrown when database operations fail
 */
class DatabaseError extends SessionError {
  constructor(operation: string, reason: string, details?: any) {
    super('DATABASE_ERROR', `Database operation '${operation}' failed: ${reason}`, 500, details);
  }

/**
 * Error thrown when session limit is exceeded
 */
class SessionLimitExceededError extends SessionError {
  constructor(limit: number, current: number, details?: any) {
    super(
      'SESSION_LIMIT_EXCEEDED',
      `Session limit exceeded. Maximum: ${limit}, Current: ${current}`,
      429,
      details
    );
  }

/**
 * Error thrown when rate limit is exceeded
 */
class RateLimitError extends SessionError {
  public readonly retryAfter?: number;

  constructor(message: string, retryAfter?: number, details?: any) {
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
