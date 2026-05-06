#include "config/manager.js.hpp"
#include "core/gateway.js.hpp"
#include "transports/http-wrapper.js.hpp"
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



using TransportMode = std::variant<'stdio', 'sse'>;

/**
 * Main entry point for the MCP Gateway Server
 */
std::future<void> main();

/**
 * Create a logger with the specified log level
 */
Console createLogger(const std::string& logLevel);

/**
 * Display help information
 */
void showHelp();

// Handle help flag

// Start the server

} // namespace elizaos
