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
 * Common utility functions
 */

;

/**
 * Format duration in seconds to human readable format
 */


  const isNegative = seconds < 0;
  const absSeconds = Math.abs(seconds);

  const days = Math.floor(absSeconds / 86400);
  const hours = Math.floor((absSeconds % 86400) / 3600);
  const minutes = Math.floor((absSeconds % 3600) / 60);
  const secs = absSeconds % 60;

  let result = '';

  if (days > 0) {
    result = `${days}d ${hours}h ${minutes}m ${secs}s`;
  } else if (hours > 0) {
    result = `${hours}h ${minutes}m ${secs}s`;
  } else if (minutes > 0) {
    result = `${minutes}m ${secs}s`;
  } else {
    result = `${secs}s`;
  }

  return isNegative ? `-${result}` : result;
}

/**
 * Truncate string to specified length
 */

  return str.substring(0, maxLength - 3) + '...';
}

/**
 * Calculate hash of data
 */


/**
 * Retry options interface
 */
struct RetryOptions {
    std::optional<double> retries;
    std::optional<double> delay;
    std::optional<(error: Error, attempt: number) => void> onRetry;
};


/**
 * Retry an async operation
 */
async ): Promise<T> {
  const { retries = 3, delay = 1000, onRetry } = options;

  let lastError: Error;

  for (let attempt = 0; attempt <= retries; attempt++) {
    try {
      return await fn();
    } catch (error) {
      lastError = error as Error;

      if (attempt < retries) {
        if (onRetry) {
          onRetry(lastError, attempt + 1);
        }

        if (delay > 0) {
          await new Promise((resolve) => setTimeout(resolve, delay));
        }
      }
    }
  }

  throw lastError!;
}

/**
 * Debounce a function
 */


    timeoutId = setTimeout(() => {
      fn(...args);
      timeoutId = null;
    }, delay);
  };
}

/**
 * Throttle a function
 */
 else {
      // Schedule the call for later if we're still within the throttle period
      if (timeoutId) {
        clearTimeout(timeoutId);
      }

      const remainingTime = delay - timeSinceLastCall;
      timeoutId = setTimeout(() => {
        fn(...args);
        lastCallTime = Date.now();
        timeoutId = null;
      }, remainingTime);
    }
  };
}

} // namespace elizaos
