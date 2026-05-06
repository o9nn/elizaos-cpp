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

/**
 * Structured error type for ElizaOS API responses
 */

/**
 * Creates a structured ElizaOS API error
 */
ElizaOSAPIError createElizaOSError(const std::string& message, double status, std::optional<std::string> code, std::optional<std::any> details, auto retryable = false);

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
std::string getAPIUrl(IAgentRuntime runtime);

/**
 * Get ElizaOS API key from runtime settings
 */
string | undefined getAPIKey(IAgentRuntime runtime);

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
 * const isAvailable = await checkElizaOSAPI(runtime);
 * if (isAvailable) {
 *   console.log('ElizaOS API is ready');
 * }
 * ```
 */
std::future<bool> checkElizaOSAPI(IAgentRuntime runtime);

} // namespace elizaos
