#include "elizaos/core.hpp"
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
 * ElizaOS API Provider
 * Calls the centralized ElizaOS API service instead of direct provider APIs
 */

;

/**
 * Structured error type for ElizaOS API responses
 */
interface ElizaOSAPIError extends Error {
  code: string;
  status: number;
  details?: any;
  retryable?: boolean;
  requestId?: string;
}

/**
 * Creates a structured ElizaOS API error
 */
`;
  error.status = status;
  error.details = details;
  error.retryable = retryable;
  return error;
}

struct ElizaOSAPIResponse {
    std::string id;
    'chat.completion' | 'chat.completion.chunk' object;
    double created;
    std::string model;
    Array<{ choices;
    double index;
    { message;
    'system' | 'user' | 'assistant' | 'tool' role;
    string | null content;
    std::optional<Array<{> tool_calls;
    std::string id;
    'function' type;
    { function;
    std::string name;
    std::string arguments;
    'stop' | 'length' | 'tool_calls' | 'content_filter' | null finish_reason;
    std::optional<{> usage;
    double prompt_tokens;
    double completion_tokens;
    double total_tokens;
    double prompt_cost;
    double completion_cost;
    double total_cost;
    std::optional<std::string> system_fingerprint;
};


struct ElizaOSChatCompletionRequest {
    std::string model;
    Array<{ messages;
    'system' | 'user' | 'assistant' | 'tool' role;
    'text' | 'image_url' type;
    std::optional<std::string> text;
    std::optional<{> image_url;
    std::string url;
    std::optional<'low' | 'high' | 'auto'> detail;
    std::optional<std::string> name;
    std::optional<Array<{> tool_calls;
    std::string id;
    'function' type;
    { function;
    std::string name;
    std::string arguments;
    std::optional<std::string> tool_call_id;
    std::optional<double> max_tokens;
    std::optional<double> temperature;
    std::optional<double> top_p;
    std::optional<double> frequency_penalty;
    std::optional<double> presence_penalty;
    std::optional<std::vector<string | string>> stop;
    std::optional<bool> stream;
    std::optional<Array<{> tools;
    'function' type;
    { function;
    std::string name;
    std::string description;
    std::unordered_map<std::string, std::any> parameters;
    'function' type;
    { name: string } function;
    std::optional<std::string> user;
};


/**
 * Get ElizaOS API URL from runtime settings
 */


/**
 * Get ElizaOS API key from runtime settings
 */


/**
 * Makes a chat completion request to the ElizaOS API service
 *
 * @param runtime - Agent runtime for configuration access
 * @param request - OpenAI-compatible chat completion request
 * @returns Promise resolving to the API response
 * @throws {ElizaOSAPIError} When API request fails
 *
 * @example
 * ```typescript
 * const response = await makeElizaOSRequest(runtime, {
 *   model: 'gpt-4o-mini',
 *   messages: [{ role: 'user', content: 'Hello' }]
 * });
 * ```
 */
std::future<ElizaOSAPIResponse> makeElizaOSRequest(IAgentRuntime runtime, ElizaOSChatCompletionRequest request);

  const url = `${apiUrl}/api/v1/chat/completions`;
  const requestId = generateRequestId();

  logger.debug(`Making request to ElizaOS API: ${url} [${requestId}]`);
  logger.debug(`Request payload:`, request);

  try {
    const response = await fetch(url, {
      method: 'POST',
      headers: {
        Authorization: `Bearer ${apiKey}`,
        'Content-Type': 'application/json',
        'X-Request-ID': requestId,
      },
      body: JSON.stringify(request),
    });

    if (!response.ok) {
      let errorDetails: any;
      let errorText: string;

      try {
        errorDetails = await response.json();
        errorText = errorDetails.error?.message || response.statusText;
      } catch {
        errorText = (await response.text()) || response.statusText;
      }

      logger.error(`ElizaOS API error ${response.status}: ${errorText} [${requestId}]`);

      const isRetryable = response.status >= 500 || response.status === 429;

      throw createElizaOSError(
        errorText,
        response.status,
        errorDetails?.error?.code || `HTTP_${response.status}`,
        errorDetails,
        isRetryable
      );
    }

    const data = await response.json();
    logger.debug(`ElizaOS API response: [${requestId}]`, data);

    return data;
  } catch (error) {
    if (error instanceof Error && error.name === 'ElizaOSAPIError') {
      throw error;
    }

    logger.error(`ElizaOS API request failed: [${requestId}]`, error);

    throw createElizaOSError(
      `Network error: ${error instanceof Error ? error.message : 'Unknown error'}`,
      0,
      'NETWORK_ERROR',
      { originalError: error },
      true
    );
  }
}

/**
 * Generates a unique request ID for tracing
 */
_${Math.random().toString(36).substr(2, 9)}`;
}

/**
 * Get the appropriate model for a given model type
 */

}

/**
 * Check if ElizaOS API is available and accessible
 *
 * @param runtime - Agent runtime for configuration access
 * @returns Promise resolving to true if API is accessible, false otherwise
 *
 * @example
 * ```typescript
 * const isAvailable = await checkElizaOSAPI(runtime);
 * if (isAvailable) {
 *   console.log('ElizaOS API is ready');
 * }
 * ```
 */
std::future<bool> checkElizaOSAPI(IAgentRuntime runtime);

  try {
    const response = await fetch(`${apiUrl}/api/models`, {
      headers: {
        Authorization: `Bearer ${apiKey}`,
        'Content-Type': 'application/json',
      },
      // Add timeout to prevent hanging
      signal: AbortSignal.timeout(10000), // 10 second timeout
    });

    if (response.ok) {
      logger.debug('ElizaOS API health check passed');
      return true;
    } else {
      logger.warn(`ElizaOS API health check failed: ${response.status}`);
      return false;
    }
  } catch (error) {
    logger.warn(
      `ElizaOS API health check error: ${error instanceof Error ? error.message : 'Unknown error'}`
    );
    return false;
  }
}

} // namespace elizaos
