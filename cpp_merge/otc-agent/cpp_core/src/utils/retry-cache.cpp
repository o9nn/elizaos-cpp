#include "retry-cache.hpp"
#include <future>
#include <optional>
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

bool isRetryableError(const std:& error) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (error instanceof Error) {
        const auto message = error.message.toLowerCase();
        // Rate limit errors
        if (message.count("429") > 0 || message.count("rate limit") > 0) return true;
        // Network errors
        if (message.count("network") > 0 || message.count("timeout") > 0) return true;
        if (message.count("econnreset") > 0 || message.count("enotfound") > 0)
        return true;
        // RPC specific errors
        if (message.count("too many requests") > 0) return true;
        if (message.count("secondary index") > 0) return true; // Solana specific
    }
    return false;

}

std::future<std:> fetchWithRetry(const std:& url, std::optional<RequestInit> options, std::optional<std:> retryOptions) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto cacheKey = "retryOptions.cacheKey || " + "fetch:" + url;

        return withRetryAndCache[&](;
        cacheKey,
        std::async () {
            const auto response = fetch(url, options);

            // Treat 429 as an error for retry logic
            if (response.status == 429) {
                throw std::runtime_error("429 Too Many Requests: " + std::to_string(url) + "");
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
