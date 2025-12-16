#include "types.ts.hpp"
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

;
;

class ApiError extends Error {
  constructor(
    message: string,
    public statusCode?: number,
  ) {
    super(message);
    this.name = "ApiError";
  }
}

const validateApiKey = (config: SearchPluginConfig): void => {
  if (!config.apiKey) {
    throw new ApiError("API key is required");
  }
};

const validateSearchQuery = (content: Content): string => {
  const query = typeof content === "string" ? content : content.text;
  if (!query?.trim()) {
    throw new ApiError("Search query is required");
  }
  return query.trim();
};

const handleApiError = (
  error: unknown,
): { success: false; response: string } => {
  if (error instanceof ApiError) {
    return {
      success: false,
      response: `API Error: ${error.message}`,
    };
  }
  return {
    success: false,
    response: "An unexpected error occurred",
  };
};

const formatSearchResults = (results: SearchResult[]): string => {
  return results
    .map((result, index) => {
      return `${index + 1}. ${result.title}\n   ${result.url}\n   ${result.snippet}\n`;
    })
    .join("\n");
};

const createRateLimiter = (maxRequests: number, timeWindow: number) => {
  const requests: number[] = [];

  return {
    checkLimit: (): boolean => {
      const now = Date.now();
      const windowStart = now - timeWindow;

      // Remove old requests
      while (requests.length > 0 && requests[0] < windowStart) {
        requests.shift();
      }

      // Check if we're at the limit
      if (requests.length >= maxRequests) {
        return false;
      }

      // Add new request
      requests.push(now);
      return true;
    },
  };
};

} // namespace elizaos
