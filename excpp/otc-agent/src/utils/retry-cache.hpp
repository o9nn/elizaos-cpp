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
 * Shared utility for exponential retry with jitter and TTL-based caching.
 * Used by RPC and CoinGecko calls to prevent rate limiting and improve performance.
 */

// In-memory cache with TTL
interface CacheEntry<T> {
  value: T;
  expiresAt: number;
}

const cache = new Map<string, CacheEntry<unknown>>();

const DEFAULT_CACHE_TTL_MS = 30_000; // 30 seconds
const MAX_RETRIES = 3;
const BASE_DELAY_MS = 1000; // 1 second
const MAX_DELAY_MS = 10_000; // 10 seconds

/**
 * Get a cached value if it exists and hasn't expired
 */


  return entry.value;
}

/**
 * Set a value in the cache with TTL
 */
);
}

/**
 * Calculate delay with exponential backoff and jitter
 */


/**
 * Check if an error is retryable (rate limit, network error, etc.)
 */

  return false;
}

/**
 * Execute a  = {},
): Promise<T> {
  const {
    maxRetries = MAX_RETRIES,
    cacheTtlMs = DEFAULT_CACHE_TTL_MS,
    skipCache = false,
  } = options;

  // Check cache first (unless skipped)
  if (!skipCache) {
    const cached = getCached<T>(cacheKey);
    if (cached !== null) {
      return cached;
    }
  }

  let lastError: Error | null = null;

  for (let attempt = 0; attempt <= maxRetries; attempt++) {
    try {
      const result = await fn();
      // Cache the successful result
      if (!skipCache) {
        setCache(cacheKey, result, cacheTtlMs);
      }
      return result;
    } catch (error) {
      lastError = error instanceof Error ? error : new Error(String(error));

      // Don't retry on non-retryable errors
      if (!isRetryableError(error)) {
        throw lastError;
      }

      // Don't delay after the last attempt
      if (attempt < maxRetries) {
        const delay = calculateDelay(attempt);
        console.warn(
          `[Retry] Attempt ${attempt + 1}/${maxRetries + 1} failed, retrying in ${Math.round(delay)}ms...`,
          lastError.message,
        );
        await new Promise((resolve) => setTimeout(resolve, delay));
      }
    }
  }

  throw lastError || new Error("All retry attempts failed");
}

/**
 * Fetch with exponential retry (for HTTP requests)
 * Automatically handles 429 and network errors
 */
std::future<Response> fetchWithRetry(const std::string& url, std::optional<RequestInit> options, std::optional<{
    maxRetries: number;
    cacheTtlMs: number;
    cacheKey: string;
  }> retryOptions);`;

  return withRetryAndCache(
    cacheKey,
    async () => {
      const response = await fetch(url, options);

      // Treat 429 as an error for retry logic
      if (response.status === 429) {
        throw new Error(`429 Too Many Requests: ${url}`);
      }

      return response;
    },
    {
      maxRetries: retryOptions?.maxRetries,
      cacheTtlMs: retryOptions?.cacheTtlMs,
      // Don't cache the Response object itself, just use retry
      skipCache: true,
    },
  );
}

/**
 * Fetch JSON with retry and caching
 * Caches the parsed JSON response
 */
async ,
): Promise<T> {
  const cacheKey = retryOptions?.cacheKey || `json:${url}`;

  // Check cache first
  const cached = getCached<T>(cacheKey);
  if (cached !== null) {
    return cached;
  }

  const response = await fetchWithRetry(url, options, {
    ...retryOptions,
    cacheKey: `fetch:${url}`, // Separate cache key for fetch
  });

  if (!response.ok) {
    throw new Error(`HTTP ${response.status}: ${response.statusText}`);
  }

  const data = await response.json();

  // Cache the parsed JSON
  setCache(cacheKey, data, retryOptions?.cacheTtlMs);

  return data as T;
}

} // namespace elizaos
