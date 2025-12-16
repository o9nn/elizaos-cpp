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
 * Server process state management
 */
struct ServerState {
    Subprocess | null process;
    bool isRunning;
};

/**
 * Global server state
 */

/**
 * Check if a local CLI exists and return its path
 */
std::future<string | null> getLocalCliPath();

/**
 * Set up environment with proper module resolution paths
 */
std::unordered_map<std::string, std::string> setupEnvironment();

/**
 * Start the server process with the given arguments
 */
std::future<void> startServerProcess(const std::vector<string[] =>& args);

/**
 * Stop the currently running server process
 */
std::future<bool> stopServerProcess();

/**
 * Restart the server process
 */
std::future<void> restartServerProcess(const std::vector<string[] =>& args);

/**
 * Check if the server is currently running
 */
bool isServerRunning();

/**
 * Get the current server process
 */

/**
 * Server process manager implementation using functional patterns
 */
    return startServerProcess(args);

    return stopServerProcess();

    return restartServerProcess(args);

    return getServerProcess();

    return isServerRunning();

/**
 * Global server manager instance
 */

/**
 * Get the global server manager instance
 */
ServerProcess getServerManager();

/**
 * Stop the server and cleanup
 * @returns true if a server was stopped, false if no server was running
 */
std::future<bool> stopServer();

/**
 * Start the server with given arguments
 */
std::future<void> startServer(const std::vector<string[] =>& args);

/**
 * Restart the server with given arguments
 */
std::future<void> restartServer(const std::vector<string[] =>& args);

/**
 * Check if the server is currently running
 */
bool isRunning();

/**
 * Get the current server process
 */

// Export functional interface for backwards compatibility

/**
 * Create a new server manager instance (factory function)
 * @deprecated Use createServerManager() instead
 */
ServerProcess DevServerManager();

} // namespace elizaos
