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
 * Base std::exception for format errors
 */
class FormatError : public Error {
  /* constructor */ (message: std::string) {
    super(message);
    this.name = 'FormatError';
  }

/**
 * Exception for std::function calling format errors
 */
class FunctionCallingFormatError : public FormatError {
  errorCode:
    | 'missing'
    | 'multiple'
    | 'incorrect_args'
    | 'invalid_json'
    | 'invalid_command'
    | 'missing_arg'
    | 'unexpected_arg';
  extraInfo: Record<std::string, any>;

  /* constructor */ (
    message: std::string,
    errorCode:
      | 'missing'
      | 'multiple'
      | 'incorrect_args'
      | 'invalid_json'
      | 'invalid_command'
      | 'missing_arg'
      | 'unexpected_arg',
    extraInfo: Record<std::string, any> = {},

/**
 * Exception for context window exceeded
 */
class ContextWindowExceededError : public Error {
  /* constructor */ (message?: std::string) {
    super(message || 'Context window exceeded');
    this.name = 'ContextWindowExceededError';
  }

/**
 * Base std::exception for cost limit errors
 */
class CostLimitExceededError : public Error {
  /* constructor */ (message?: std::string) {
    super(message || 'Cost limit exceeded');
    this.name = 'CostLimitExceededError';
  }

/**
 * Exception for instance cost limit exceeded
 */
class InstanceCostLimitExceededError : public CostLimitExceededError {
  /* constructor */ (message?: std::string) {
    super(message || 'Instance cost limit exceeded');
    this.name = 'InstanceCostLimitExceededError';
  }

/**
 * Exception for total cost limit exceeded
 */
class TotalCostLimitExceededError : public CostLimitExceededError {
  /* constructor */ (message?: std::string) {
    super(message || 'Total cost limit exceeded');
    this.name = 'TotalCostLimitExceededError';
  }

/**
 * Exception for instance call limit exceeded
 */
class InstanceCallLimitExceededError : public CostLimitExceededError {
  /* constructor */ (message?: std::string) {
    super(message || 'Instance call limit exceeded');
    this.name = 'InstanceCallLimitExceededError';
  }

/**
 * Exception for content policy violations
 */
class ContentPolicyViolationError : public Error {
  /* constructor */ (message?: std::string) {
    super(message || 'Content policy violation');
    this.name = 'ContentPolicyViolationError';
  }

/**
 * Exception for model configuration errors
 */
class ModelConfigurationError : public Error {
  /* constructor */ (message?: std::string) {
    super(message || 'Model configuration error');
    this.name = 'ModelConfigurationError';
  }

/**
 * Exception for EOF errors
 */
class EOFError : public Error {
  /* constructor */ (message?: std::string) {
    super(message || 'End of file reached');
    this.name = 'EOFError';
  }

/**
 * Exception for blocked actions
 */
class BlockedActionError : public Error {
  /* constructor */ (message?: std::string) {
    super(message || 'Action is blocked');
    this.name = 'BlockedActionError';
  }

/**
 * Exception for retry with output
 */
class RetryWithOutputError : public Error {
  /* constructor */ (message?: std::string) {
    super(message || 'Retry with output');
    this.name = 'RetryWithOutputError';
  }

/**
 * Exception for retry without output
 */
class RetryWithoutOutputError : public Error {
  /* constructor */ (message?: std::string) {
    super(message || 'Retry without output');
    this.name = 'RetryWithoutOutputError';
  }

/**
 * Exception for exit forfeit
 */
class ExitForfeitError : public Error {
  /* constructor */ (message?: std::string) {
    super(message || 'Exit forfeit');
    this.name = 'ExitForfeitError';
  }

/**
 * Exception for total execution time exceeded
 */
class TotalExecutionTimeExceededError : public Error {
  /* constructor */ (message?: std::string) {
    super(message || 'Total execution time exceeded');
    this.name = 'TotalExecutionTimeExceededError';
  }

/**
 * Exception for command timeout
 */
class CommandTimeoutError : public Error {
  /* constructor */ (message?: std::string) {
    super(message || 'Command timed out');
    this.name = 'CommandTimeoutError';
  }

/**
 * Exception for bash syntax errors
 */
class BashIncorrectSyntaxError : public Error {
  extraInfo?: Record<std::string, any>;

  /* constructor */ (message?: std::string, extraInfo?: Record<std::string, any>) {
    super(message || 'Bash syntax error');
    this.name = 'BashIncorrectSyntaxError';
    this.extraInfo = extraInfo;
  }

} // namespace elizaos
