#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

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
class FormatError {
public:
    FormatError(const std::string& message);
};

/**
 * Exception for function calling format errors
 */
class FunctionCallingFormatError {
public:
    FunctionCallingFormatError(const std::string& message, const std::variant<, 'missing', 'multiple', 'incorrect_args', 'invalid_json', 'invalid_command', 'missing_arg', 'unexpected_arg'>& errorCode, Record<string extraInfo, auto any> = {});

/**
 * Exception for context window exceeded
 */
class ContextWindowExceededError {
public:
    ContextWindowExceededError(std::optional<std::string> message);
};

/**
 * Base exception for cost limit errors
 */
class CostLimitExceededError {
public:
    CostLimitExceededError(std::optional<std::string> message);
};

/**
 * Exception for instance cost limit exceeded
 */
class InstanceCostLimitExceededError {
public:
    InstanceCostLimitExceededError(std::optional<std::string> message);
};

/**
 * Exception for total cost limit exceeded
 */
class TotalCostLimitExceededError {
public:
    TotalCostLimitExceededError(std::optional<std::string> message);
};

/**
 * Exception for instance call limit exceeded
 */
class InstanceCallLimitExceededError {
public:
    InstanceCallLimitExceededError(std::optional<std::string> message);
};

/**
 * Exception for content policy violations
 */
class ContentPolicyViolationError {
public:
    ContentPolicyViolationError(std::optional<std::string> message);
};

/**
 * Exception for model configuration errors
 */
class ModelConfigurationError {
public:
    ModelConfigurationError(std::optional<std::string> message);
};

/**
 * Exception for EOF errors
 */
class EOFError {
public:
    EOFError(std::optional<std::string> message);
};

/**
 * Exception for blocked actions
 */
class BlockedActionError {
public:
    BlockedActionError(std::optional<std::string> message);
};

/**
 * Exception for retry with output
 */
class RetryWithOutputError {
public:
    RetryWithOutputError(std::optional<std::string> message);
};

/**
 * Exception for retry without output
 */
class RetryWithoutOutputError {
public:
    RetryWithoutOutputError(std::optional<std::string> message);
};

/**
 * Exception for exit forfeit
 */
class ExitForfeitError {
public:
    ExitForfeitError(std::optional<std::string> message);
};

/**
 * Exception for total execution time exceeded
 */
class TotalExecutionTimeExceededError {
public:
    TotalExecutionTimeExceededError(std::optional<std::string> message);
};

/**
 * Exception for command timeout
 */
class CommandTimeoutError {
public:
    CommandTimeoutError(std::optional<std::string> message);
};

/**
 * Exception for bash syntax errors
 */
class BashIncorrectSyntaxError {
public:
    BashIncorrectSyntaxError(std::optional<std::string> message, std::optional<Record<string> extraInfo, auto any>);
};


} // namespace elizaos
