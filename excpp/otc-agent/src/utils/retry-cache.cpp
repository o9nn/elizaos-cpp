#include "retry-cache.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

double calculateDelay(double attempt) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Exponential backoff: 1s, 2s, 4s, ...
    const auto exponentialDelay = BASE_DELAY_MS * Math.pow(2, attempt);
    // Add jitter (±25%)
    const auto jitter = exponentialDelay * 0.25 * (Math.random() * 2 - 1);
    const auto delay = Math.min(exponentialDelay + jitter, MAX_DELAY_MS);
    return Math.max(delay, BASE_DELAY_MS);

}

bool isRetryableError(unknown error) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (error instanceof Error) {
        const auto message = error.message.toLowerCase();
        // Rate limit errors
        if (message.includes("429") || message.includes("rate limit")) return true;
        // Network errors
        if (message.includes("network") || message.includes("timeout")) return true;
        if (message.includes("econnreset") || message.includes("enotfound"))
        return true;
        // RPC specific errors
        if (message.includes("too many requests")) return true;
        if (message.includes("secondary index")) return true; // Solana specific
    }
    return false;

}

std::future<Response> fetchWithRetry(const std::string& url, std::optional<RequestInit> options, std::optional<std::any> retryOptions) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto cacheKey = "fetch:" + std::to_string(url);

        return withRetryAndCache(;
        cacheKey,
        async () => {
            const auto response = fetch(url, options);

            // Treat 429 as an error for retry logic
            if (response.status == 429) {
                throw std::runtime_error(`429 Too Many Requests: ${url}`);
            }

            return response;
            },
            {
                maxRetries: retryOptions.maxRetries,
                cacheTtlMs: retryOptions.cacheTtlMs,
                // Don't cache the Response object itself, just use retry
                skipCache: true,
                },
                );

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
