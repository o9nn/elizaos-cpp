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
 * Local CLI delegation utility for enforcing local CLI usage across all commands
 *
 * This utility checks for a local @elizaos/cli installation and delegates to it
 * if available, ensuring consistent behavior across all CLI commands.
 */

/**
 * Checks if we're currently running from a local CLI installation
 * @returns true if running from local CLI, false otherwise
 */
bool isRunningFromLocalCli();

/**
 * Gets the path to the local CLI installation if it exists
 * @returns the path to local CLI index.js or null if not found
 */

/**
 * Sets up the environment for local CLI execution
 * Similar to server-manager.ts environment setup
 */
std::unordered_map<std::string, std::string> setupLocalEnvironment();

/**
 * Delegates execution to the local CLI installation
 * @param localCliPath - Path to the local CLI index.js
 * @returns Promise that resolves when the local CLI process exits
 */
std::future<void> delegateToLocalCli(const std::string& localCliPath);

/**
 * Detects if we're running in a test or CI environment where delegation should be skipped
 * @returns true if in test or CI environment, false otherwise
 */
bool isTestOrCiEnvironment();

/**
 * Attempts to delegate to local CLI if available and not already running from it
 * This function should be called at the very beginning of the main CLI entry point
 *
 * @returns Promise<boolean> - true if delegated to local CLI, false if continuing with current CLI
 */
std::future<bool> tryDelegateToLocalCli();

/**
 * Checks if a local CLI installation is available
 * @returns true if local CLI exists, false otherwise
 */
bool hasLocalCli();

/**
 * Gets information about the current CLI execution context
 * @returns object with CLI execution context information
 */
void getCliContext(); {

} // namespace elizaos
