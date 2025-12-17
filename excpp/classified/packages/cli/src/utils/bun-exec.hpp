#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Constants

/**
 * Helper to ensure bun is in PATH for subprocess execution
 */
std::unordered_map<std::string, std::string> ensureBunInPath(std::unordered_map<std::string, std::string> env = {});

struct ExecResult {
    std::string stdout;
    std::string stderr;
    std::optional<double> exitCode;
    bool success;
};

struct BunExecOptions {
    std::optional<std::string> cwd;
    std::optional<std::string> stdio;
    std::optional<std::string> stdout;
    std::optional<std::string> stderr;
    std::optional<double> timeout;
    std::optional<AbortSignal> signal;
};

// Define proper error types for better error handling
class ProcessExecutionError {
public:
    ProcessExecutionError(const std::string& message);
};

class ProcessTimeoutError {
public:
    ProcessTimeoutError(const std::string& message);
};

/**
 * Properly escape shell arguments to prevent command injection
 *
 * @param arg - The argument to escape
 * @returns The escaped argument
 *
 * @example
 * ```typescript
 * escapeShellArg('hello world') // => '"hello world"'
 * escapeShellArg('hello"world') // => '"hello\\"world"'
 * escapeShellArg('') // => '""'
 * ```
 */
std::string escapeShellArg(const std::string& arg);

/**
 * Helper to read a stream into a string
 */
std::future<std::string> readStreamSafe(const std::variant<ReadableStream, double>& stream, const std::string& streamName);

/**
 * Execute a command using Bun's shell functionality with enhanced security and error handling
 * This is a drop-in replacement for execa
 *
 * @param command - The command to execute
 * @param args - Array of arguments to pass to the command
 * @param options - Execution options including cwd, env, stdio, timeout, and signal
 * @returns Promise resolving to execution result with stdout, stderr, exitCode, and success
 *
 * @throws {ProcessExecutionError} When the command execution fails
 * @throws {ProcessTimeoutError} When the command times out
 *
 * @example
 * ```typescript
 * // Simple command execution
 *
 * // With timeout
 * try {
 * } catch (error) {
 *   if (error instanceof ProcessTimeoutError) {
 *   }
 * }
 *
 * // With custom environment
 *   cwd: '/path/to/project',
 *   env: { NODE_ENV: 'production' }
 * });
 * ```
 */
std::future<ExecResult> bunExec(const std::string& command, std::vector<std::string> args = {}, BunExecOptions options = {});

/**
 * Execute a command and only return stdout (similar to execa's simple usage)
 * Throws an error if the command fails unless stdio is set to 'ignore'
 *
 * @param command - The command to execute
 * @param args - Array of arguments to pass to the command
 * @param options - Execution options
 * @returns Promise resolving to object with stdout property
 *
 *
 * @example
 * ```typescript
 * // Get command output
 *
 * // Ignore errors
 * ```
 */

/**
 * Execute a command with inherited stdio (for interactive commands)
 * Useful for commands that need user interaction or should display output directly
 *
 * @param command - The command to execute
 * @param args - Array of arguments to pass to the command
 * @param options - Execution options (stdio will be overridden to 'inherit')
 * @returns Promise resolving to execution result
 *
 * @example
 * ```typescript
 * // Run interactive command
 *
 * // Run command that needs terminal colors
 * ```
 */
std::future<ExecResult> bunExecInherit(const std::string& command, std::vector<std::string> args = {}, BunExecOptions options = {});

/**
 * Check if a command exists in the system PATH
 * Uses 'which' on Unix-like systems and 'where' on Windows
 *
 * @param command - The command name to check
 * @returns Promise resolving to true if command exists, false otherwise
 *
 * @example
 * ```typescript
 * // Check if git is installed
 * } else {
 * }
 *
 * // Check for optional tools
 * ```
 */
std::future<bool> commandExists(const std::string& command);

} // namespace elizaos
