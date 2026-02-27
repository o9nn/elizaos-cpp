#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
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
    std: command;
    std::optional<double> timeout;
    std::optional<std:> check;
};

/**
 * Bash action result
 */
struct BashActionResult {
    std: output;
    double exitCode;
};

/**
 * Interrupt action for stopping running commands
 */
struct BashInterruptAction {
    std: type;
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
    std: command;
    std::optional<bool> shell;
    std::optional<bool> check;
    std::optional<std:> cwd;
    std::optional<double> timeout;
};

/**
 * Command execution result
 */
struct CommandResult {
    double exitCode;
    std: stdout;
    std: stderr;
};

/**
 * File read request
 */
struct ReadFileRequest {
    std: path;
    std::optional<std:> encoding;
    std::optional<std:> errors;
};

/**
 * File read response
 */
struct ReadFileResponse {
    std: content;
};

/**
 * File write request
 */
struct WriteFileRequest {
    std: path;
    std: content;
};

/**
 * Upload request for copying files/directories
 */
struct UploadRequest {
    std: sourcePath;
    std: targetPath;
};

/**
 * Abstract runtime interface
 */


} // namespace elizaos
