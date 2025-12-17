#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Server process manager for development mode
 *
 * Manages the lifecycle of the development server process, including starting, stopping, and restarting.
 */
class DevServerManager {
public:
    std::future<void> stop();
    std::future<void> start(const std::vector<string[] =>& args);
    std::future<void> restart(const std::vector<string[] =>& args);
};

// Global server instance

/**
 * Get the global server manager instance
 */
DevServerManager getServerManager();

/**
 * Stop the server and cleanup
 */
std::future<void> stopServer();

/**
 * Start the server with given arguments
 */
std::future<void> startServer(const std::vector<string[] =>& args);

/**
 * Restart the server with given arguments
 */
std::future<void> restartServer(const std::vector<string[] =>& args);

} // namespace elizaos
