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
export function getCached<T>(key: string): T | null {
  const entry = cache.get(key) as CacheEntry<T> | undefined;
  if (!entry) return null;

  if (Date.now() > entry.expiresAt) {
    cache.delete(key);
    return null;
  }

  return entry.value;
}

/**
 * Set a value in the cache with TTL
 */
export function setCache<T>(
  key: string,
  value: T,
  ttlMs: number = DEFAULT_CACHE_TTL_MS,
): void {
  cache.set(key, {
    value,
    expiresAt: Date.now() + ttlMs,
  });
}

/**
 * Calculate delay with exponential backoff and jitter
 */
function calculateDelay(attempt: number): number {
  // Exponential backoff: 1s, 2s, 4s, ...
  const exponentialDelay = BASE_DELAY_MS * Math.pow(2, attempt);
  // Add jitter (±25%)
  const jitter = exponentialDelay * 0.25 * (Math.random() * 2 - 1);
  const delay = Math.min(exponentialDelay + jitter, MAX_DELAY_MS);
  return Math.max(delay, BASE_DELAY_MS);
}

/**
 * Check if an error is retryable (rate limit, network error, etc.)
 */
function isRetryableError(error: unknown): boolean {
  if (error instanceof Error) {
    const message = error.message.toLowerCase();
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

/**
 * Execute a function with exponential retry and caching
 * @param cacheKey Unique key for caching the result
 * @param fn The async function to execute
 * @param options Configuration options
 */
export async function withRetryAndCache<T>(
  cacheKey: string,
  fn: () => Promise<T>,
  options: {
    maxRetries?: number;
    cacheTtlMs?: number;
    skipCache?: boolean;
  } = {},
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
export async function fetchWithRetry(
  url: string,
  options?: RequestInit,
  retryOptions?: {
    maxRetries?: number;
    cacheTtlMs?: number;
    cacheKey?: string;
  },
): Promise<Response> {
  const cacheKey = retryOptions?.cacheKey || `fetch:${url}`;

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
export async function fetchJsonWithRetryAndCache<T>(
  url: string,
  options?: RequestInit,
  retryOptions?: {
    maxRetries?: number;
    cacheTtlMs?: number;
    cacheKey?: string;
  },
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
