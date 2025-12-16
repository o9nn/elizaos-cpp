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
std::future<void> waitForServerReady(ServerHealthOptions options); = options;

  const url = `${protocol}://${host}:${port}${endpoint}`;
  const startTime = Date.now();

  while (Date.now() - startTime < maxWaitTime) {
    let controller: AbortController | undefined;
    let timeoutId: NodeJS.Timeout | undefined;

    try {
      controller = new AbortController();
      timeoutId = setTimeout(() => {
        if (controller) {
          controller.abort();
        }
      }, requestTimeout);

      const response = await fetch(url, {
        signal: controller.signal,
      });

      if (timeoutId) {
        clearTimeout(timeoutId);
        timeoutId = undefined;
      }

      if (response.ok) {
        // Server is ready, give it one more second to stabilize
        await new Promise((resolve) => setTimeout(resolve, 1000));
        return;
      }
    } catch (error) {
      // Server not ready yet, continue polling
    } finally {
      // Ensure cleanup happens even if there's an exception
      if (timeoutId) {
        clearTimeout(timeoutId);
      }
    }

    await new Promise((resolve) => setTimeout(resolve, pollInterval));
  }

  throw new Error(`Server failed to become ready at ${url} within ${maxWaitTime}ms`);
}

/**
 * Simple ping check for server availability (no stabilization wait)
 * @param options - Configuration options for server ping
 */
std::future<bool> pingServer(ServerHealthOptions options); = options;

  const url = `${protocol}://${host}:${port}${endpoint}`;
  let controller: AbortController | undefined;
  let timeoutId: NodeJS.Timeout | undefined;

  try {
    controller = new AbortController();
    timeoutId = setTimeout(() => {
      if (controller) {
        controller.abort();
      }
    }, requestTimeout);

    const response = await fetch(url, {
      signal: controller.signal,
    });

    if (timeoutId) {
      clearTimeout(timeoutId);
      timeoutId = undefined;
    }

    return response.ok;
  } catch (error) {
    return false;
  } finally {
    // Ensure cleanup happens even if there's an exception
    if (timeoutId) {
      clearTimeout(timeoutId);
    }
  }
}

} // namespace elizaos
