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
 * Server health check utilities for waiting for servers to be ready
 */

struct ServerHealthOptions {
    double port;
    std::optional<std::string> endpoint;
    std::optional<double> maxWaitTime;
    std::optional<double> pollInterval;
    std::optional<double> requestTimeout;
    std::optional<std::string> host;
    std::optional<'http' | 'https'> protocol;
};

/**
 * Wait for server to be ready by polling health endpoint
 * @param options - Configuration options for server health check
 */
std::future<void> waitForServerReady(ServerHealthOptions options);

/**
 * Simple ping check for server availability (no stabilization wait)
 * @param options - Configuration options for server ping
 */
std::future<bool> pingServer(ServerHealthOptions options);

} // namespace elizaos
