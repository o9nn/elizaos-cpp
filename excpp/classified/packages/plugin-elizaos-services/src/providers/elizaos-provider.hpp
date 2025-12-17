#pragma once
#include <any>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * ElizaOS API Provider
 * Calls the centralized ElizaOS API service instead of direct provider APIs
 */

/**
 * Structured error type for ElizaOS API responses
 */

/**
 * Creates a structured ElizaOS API error
 */
ElizaOSAPIError createElizaOSError(const std::string& message, double status, std::optional<std::string> code, std::optional<std::any> details, auto retryable);

struct ElizaOSAPIResponse {
    std::string id;
    std::string object;
    double created;
    std::string model;
    double index;
    std::string role;
    std::string content;
    std::string id;
    std::string type;
    std::string name;
    std::string arguments;
    std::string finish_reason;
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
    std::string role;
    std::string type;
    std::optional<std::string> text;
    std::string url;
    std::optional<std::string> detail;
    std::optional<std::string> name;
    std::string id;
    std::string type;
    std::string name;
    std::string arguments;
    std::optional<std::string> tool_call_id;
    std::optional<double> max_tokens;
    std::optional<double> temperature;
    std::optional<double> top_p;
    std::optional<double> frequency_penalty;
    std::optional<double> presence_penalty;
    std::optional<std::variant<std::string, std::vector<std::string>>> stop;
    std::optional<bool> stream;
    std::string type;
    std::string name;
    std::string description;
    std::string type;
    std::optional<std::string> user;
};

/**
 * Get ElizaOS API URL from runtime settings
 */
std::string getAPIUrl(IAgentRuntime runtime);

/**
 * Get ElizaOS API key from runtime settings
 */
std::string getAPIKey(IAgentRuntime runtime);

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
 *   model: 'gpt-4o-mini',
 *   messages: [{ role: 'user', content: 'Hello' }]
 * });
 * ```
 */
std::future<ElizaOSAPIResponse> makeElizaOSRequest(IAgentRuntime runtime, ElizaOSChatCompletionRequest request);

/**
 * Generates a unique request ID for tracing
 */
std::string generateRequestId();

/**
 * Get the appropriate model for a given model type
 */
std::string getModelForType(ModelTypeName modelType);

/**
 * Check if ElizaOS API is available and accessible
 *
 * @param runtime - Agent runtime for configuration access
 * @returns Promise resolving to true if API is accessible, false otherwise
 *
 * @example
 * ```typescript
 * if (isAvailable) {
 * }
 * ```
 */
std::future<bool> checkElizaOSAPI(IAgentRuntime runtime);

} // namespace elizaos
