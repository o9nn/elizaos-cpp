#pragma once
#include <algorithm>
#include <any>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * Shared utility for exponential retry with jitter and TTL-based caching.
 * Used by RPC and CoinGecko calls to prevent rate limiting and improve performance.
 */

// In-memory cache with TTL

/**
 * Get a cached value if it exists and hasn't expired
 */

/**
 * Set a value in the cache with TTL
 */

/**
 * Calculate delay with exponential backoff and jitter
 */
double calculateDelay(double attempt);

/**
 * Check if an error is retryable (rate limit, network error, etc.)
 */
bool isRetryableError(const std::any& error);

/**
 * Execute a function with exponential retry and caching
 * @param cacheKey Unique key for caching the result
 * @param fn The async function to execute
 * @param options Configuration options
 */

  // Check cache first (unless skipped)

      // Cache the successful result

      // Don't retry on non-retryable errors

      // Don't delay after the last attempt

/**
 * Fetch with exponential retry (for HTTP requests)
 * Automatically handles 429 and network errors
 */
std::future<std::string> fetchWithRetry(const std::string& url, std::optional<RequestInit> options, std::optional<std::any> retryOptions);

/**
 * Fetch JSON with retry and caching
 * Caches the parsed JSON response
 */

  // Check cache first

  // Cache the parsed JSON


} // namespace elizaos
