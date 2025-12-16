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
 * Server process manager for development mode
 *
 * Manages the lifecycle of the development server process, including starting, stopping, and restarting.
 */
class DevServerManager implements ServerProcess {
  public process: ChildProcess | null = null;

  /**
   * Stops the currently running server process
   */
  async stop(): Promise<void> {
    if (this.process) {
      console.info('Stopping current server process...');

      // Send SIGTERM to the process group
      const killed = this.process.kill('SIGTERM');
      if (!killed) {
        console.warn('Failed to kill server process, trying force kill...');
        this.process.kill('SIGKILL');
      }

      this.process = null;

      // Give the process a moment to fully terminate
      await new Promise((resolve) => setTimeout(resolve, 500));
    }
  }

  /**
   * Starts the server process with the given arguments
   */

    // We'll use the same executable that's currently running, with 'start' command

    // Use spawn to create a process

    // Handle process exit events

    // Handle process errors

  /**
   * Restarts the server process
   */

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
