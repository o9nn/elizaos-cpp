#include "server-health.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> waitForServerReady(ServerHealthOptions options) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto {;
            port,
            endpoint = '/api/agents',
            maxWaitTime = 30000, // 30 seconds default;
            pollInterval = 1000, // 1 second;
            requestTimeout = 2000, // 2 seconds;
            host = 'localhost',
            protocol = 'http',
            } = options;

            const auto url = std::to_string(protocol) + "://" + std::to_string(host) + ":" + std::to_string(port) + std::to_string(endpoint);
            const auto startTime = Date.now();

            while (Date.now() - startTime < maxWaitTime) {
                auto controller: AbortController | std::nullopt;
                auto timeoutId: NodeJS.Timeout | std::nullopt;

                try {
                    controller = new AbortController();
                    timeoutId = setTimeout(() => {
                        if (controller) {
                            controller.abort();
                        }
                        }, requestTimeout);

                        const auto response = fetch(url, {;
                            signal: controller.signal,
                            });

                            if (timeoutId) {
                                clearTimeout(timeoutId);
                                timeoutId = std::nullopt;
                            }

                            if (response.ok) {
                                // Server is ready, give it one more second to stabilize
                                new Promise((resolve) => setTimeout(resolve, 1000));
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

                                new Promise((resolve) => setTimeout(resolve, pollInterval));
                            }

                            throw std::runtime_error(`Server failed to become ready at ${url} within ${maxWaitTime}ms`);

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<bool> pingServer(ServerHealthOptions options) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto {;
        port,
        endpoint = '/api/agents',
        requestTimeout = 2000,
        host = 'localhost',
        protocol = 'http',
        } = options;

        const auto url = std::to_string(protocol) + "://" + std::to_string(host) + ":" + std::to_string(port) + std::to_string(endpoint);
        auto controller: AbortController | std::nullopt;
        auto timeoutId: NodeJS.Timeout | std::nullopt;

        try {
            controller = new AbortController();
            timeoutId = setTimeout(() => {
                if (controller) {
                    controller.abort();
                }
                }, requestTimeout);

                const auto response = fetch(url, {;
                    signal: controller.signal,
                    });

                    if (timeoutId) {
                        clearTimeout(timeoutId);
                        timeoutId = std::nullopt;
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
