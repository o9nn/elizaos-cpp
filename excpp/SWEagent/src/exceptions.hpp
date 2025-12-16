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
 * Custom exceptions
 * Converted from sweagent/exceptions.py
 */

/**
 * Base exception for format errors
 */
class FormatError extends Error {
  constructor(message: string) {
    super(message);
    this.name = 'FormatError';
  }
}

/**
 * Exception for ,
  ) {
    super(message);
    this.name = 'FunctionCallingFormatError';
    this.errorCode = errorCode;
    this.extraInfo = extraInfo;
  }
}

/**
 * Exception for context window exceeded
 */
class ContextWindowExceededError extends Error {
  constructor(message?: string) {
    super(message || 'Context window exceeded');
    this.name = 'ContextWindowExceededError';
  }
}

/**
 * Base exception for cost limit errors
 */
class CostLimitExceededError extends Error {
  constructor(message?: string) {
    super(message || 'Cost limit exceeded');
    this.name = 'CostLimitExceededError';
  }
}

/**
 * Exception for instance cost limit exceeded
 */
class InstanceCostLimitExceededError extends CostLimitExceededError {
  constructor(message?: string) {
    super(message || 'Instance cost limit exceeded');
    this.name = 'InstanceCostLimitExceededError';
  }
}

/**
 * Exception for total cost limit exceeded
 */
class TotalCostLimitExceededError extends CostLimitExceededError {
  constructor(message?: string) {
    super(message || 'Total cost limit exceeded');
    this.name = 'TotalCostLimitExceededError';
  }
}

/**
 * Exception for instance call limit exceeded
 */
class InstanceCallLimitExceededError extends CostLimitExceededError {
  constructor(message?: string) {
    super(message || 'Instance call limit exceeded');
    this.name = 'InstanceCallLimitExceededError';
  }
}

/**
 * Exception for content policy violations
 */
class ContentPolicyViolationError extends Error {
  constructor(message?: string) {
    super(message || 'Content policy violation');
    this.name = 'ContentPolicyViolationError';
  }
}

/**
 * Exception for model configuration errors
 */
class ModelConfigurationError extends Error {
  constructor(message?: string) {
    super(message || 'Model configuration error');
    this.name = 'ModelConfigurationError';
  }
}

/**
 * Exception for EOF errors
 */
class EOFError extends Error {
  constructor(message?: string) {
    super(message || 'End of file reached');
    this.name = 'EOFError';
  }
}

/**
 * Exception for blocked actions
 */
class BlockedActionError extends Error {
  constructor(message?: string) {
    super(message || 'Action is blocked');
    this.name = 'BlockedActionError';
  }
}

/**
 * Exception for retry with output
 */
class RetryWithOutputError extends Error {
  constructor(message?: string) {
    super(message || 'Retry with output');
    this.name = 'RetryWithOutputError';
  }
}

/**
 * Exception for retry without output
 */
class RetryWithoutOutputError extends Error {
  constructor(message?: string) {
    super(message || 'Retry without output');
    this.name = 'RetryWithoutOutputError';
  }
}

/**
 * Exception for exit forfeit
 */
class ExitForfeitError extends Error {
  constructor(message?: string) {
    super(message || 'Exit forfeit');
    this.name = 'ExitForfeitError';
  }
}

/**
 * Exception for total execution time exceeded
 */
class TotalExecutionTimeExceededError extends Error {
  constructor(message?: string) {
    super(message || 'Total execution time exceeded');
    this.name = 'TotalExecutionTimeExceededError';
  }
}

/**
 * Exception for command timeout
 */
class CommandTimeoutError extends Error {
  constructor(message?: string) {
    super(message || 'Command timed out');
    this.name = 'CommandTimeoutError';
  }
}

/**
 * Exception for bash syntax errors
 */
class BashIncorrectSyntaxError extends Error {
  extraInfo?: Record<string, any>;

  constructor(message?: string, extraInfo?: Record<string, any>) {
    super(message || 'Bash syntax error');
    this.name = 'BashIncorrectSyntaxError';
    this.extraInfo = extraInfo;
  }
}

} // namespace elizaos
