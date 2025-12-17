#pragma once
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
 * Runtime abstractions deployment
 * Interfaces and types matching SWE-ReX runtime
 */

/**
 * Bash action for running commands in a session
 */
struct BashAction {
    std::string command;
    std::optional<double> timeout;
    std::optional<std::string> check;
};

/**
 * Bash action result
 */
struct BashActionResult {
    std::string output;
    double exitCode;
};

/**
 * Interrupt action for stopping running commands
 */
struct BashInterruptAction {
    std::string type;
};

/**
 * Create bash session request
 */
struct CreateBashSessionRequest {
    std::optional<std::vector<std::string>> startupSource;
    std::optional<double> startupTimeout;
};

/**
 * Command execution request (non-session)
 */
struct Command {
    std::string command;
    std::optional<bool> shell;
    std::optional<bool> check;
    std::optional<std::string> cwd;
    std::optional<double> timeout;
};

/**
 * Command execution result
 */
struct CommandResult {
    double exitCode;
    std::string stdout;
    std::string stderr;
};

/**
 * File read request
 */
struct ReadFileRequest {
    std::string path;
    std::optional<std::string> encoding;
    std::optional<std::string> errors;
};

/**
 * File read response
 */
struct ReadFileResponse {
    std::string content;
};

/**
 * File write request
 */
struct WriteFileRequest {
    std::string path;
    std::string content;
};

/**
 * Upload request for copying files/directories
 */
struct UploadRequest {
    std::string sourcePath;
    std::string targetPath;
};

/**
 * Abstract runtime interface
 */


} // namespace elizaos
