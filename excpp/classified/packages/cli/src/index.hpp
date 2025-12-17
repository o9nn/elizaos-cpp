#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Shutdown state management to prevent race conditions
 * Using an object to encapsulate state and provide atomic operations
 */

  /**
   * Atomically check and set the shutdown flag
   * @returns true if shutdown was initiated, false if already in progress
   */

/**
 * Graceful shutdown handler for SIGINT and SIGTERM signals
 * Ensures proper cleanup of server processes before exiting
 * Prevents race conditions from multiple rapid signal events
 */
std::future<void> gracefulShutdown(const std::string& signal);

/**
 * Asynchronous function that serves as the main entry point for the application.
 * It loads environment variables, initializes the CLI program, and parses the command line arguments.
 * @returns {Promise<void>}
 */
std::future<void> main();


} // namespace elizaos
