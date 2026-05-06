#include "bun-exec.js.hpp"
#include "elizaos/core.hpp"
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
 * Check if quiet mode is enabled
 */
bool isQuietMode();

struct SpinnerCommandOptions {
    std::optional<std::string> cwd;
    std::string spinnerText;
    std::string successText;
    std::optional<std::string> errorText;
    std::optional<bool> showOutputOnError;
};

struct CommandResult {
    bool success;
    std::optional<std::string> stdout;
    std::optional<std::string> stderr;
    std::optional<Error> error;
};

/**
 * Core function to run any command with a spinner
 */
std::future<CommandResult> runCommandWithSpinner(const std::string& command, const std::vector<std::string>& args, SpinnerCommandOptions options);

/**
 * Run a bun command with spinner
 */
std::future<CommandResult> runBunWithSpinner(const std::vector<std::string>& args, const std::string& cwd, const std::optional<SpinnerCommandOptions>& options);

/**
 * Install dependencies with spinner
 */
std::future<void> installDependenciesWithSpinner(const std::string& targetDir);

/**
 * Build project with spinner
 */
std::future<void> buildProjectWithSpinner(const std::string& targetDir, auto isPlugin = false);

/**
 * Install plugin with spinner (non-critical, warns on failure)
 */
std::future<void> installPluginWithSpinner(const std::string& pluginName, const std::string& targetDir, auto purpose = '');

/**
 * Create a task for use with clack.tasks()
 */
      await fn();

/**
 * Run multiple tasks with clack.tasks()
 */

} // namespace elizaos
