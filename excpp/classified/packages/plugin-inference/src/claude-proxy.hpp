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
#include "index.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Claude API Types
struct ClaudeTool {
    std::string name;
    std::optional<std::string> description;
    std::any input_schema;
};

using ClaudeContent = std::variant<, std::string, Array<{

struct ClaudeMessage {
    std::string role;
    ClaudeContent content;
};

struct ClaudeMessagesRequest {
    std::string model;
    std::vector<ClaudeMessage> messages;
    std::optional<std::string> system;
    double max_tokens;
    std::optional<std::vector<std::string>> stop_sequences;
    std::optional<bool> stream;
    std::optional<double> temperature;
    std::optional<double> top_p;
    std::optional<double> top_k;
    std::optional<std::vector<ClaudeTool>> tools;
};

struct ClaudeResponse {
    std::string id;
    std::string type;
    std::string role;
    std::string model;
    std::string type;
    std::optional<std::string> text;
    std::optional<std::string> id;
    std::optional<std::string> name;
    std::optional<std::any> input;
    std::string stop_reason;
    double input_tokens;
    double output_tokens;
};

// OpenAI-style types for compatibility
struct OpenAIMessage {
    std::string role;
    std::optional<std::string> text;
    std::string id;
    std::string type;
    std::string name;
    std::string arguments;
    std::optional<std::string> tool_call_id;
};

/**
 * Claude-to-Provider Proxy
 * Converts Claude API requests to the appropriate provider format
 */
class ClaudeProxy {
public:
    ClaudeProxy();
    std::future<ClaudeResponse> processRequest(ClaudeMessagesRequest claudeRequest);
    std::future<ClaudeResponse> makeAnthropicProxyRequest(ClaudeMessagesRequest claudeRequest);
    std::future<InferenceProvider> selectProvider();
    std::future<bool> isProviderAvailable(InferenceProvider provider);
    std::future<ClaudeResponse> handleOpenAIRequest(ClaudeMessagesRequest claudeRequest);
    std::future<ClaudeResponse> handleGroqRequest(ClaudeMessagesRequest claudeRequest);
    std::future<ClaudeResponse> handleOllamaRequest(ClaudeMessagesRequest claudeRequest);
    std::vector<OpenAIMessage> convertClaudeToOpenAI(ClaudeMessagesRequest claudeRequest);
    std::string convertClaudeToPrompt(ClaudeMessagesRequest claudeRequest);
    std::string formatOpenAIPrompt(const std::vector<OpenAIMessage>& messages, std::optional<std::string> system);
    ClaudeResponse formatClaudeResponse(const std::string& text, const std::string& model);
};

/**
 * Create a Claude API handler for the inference plugin
 */
void createClaudeHandler(IAgentRuntime runtime);

} // namespace elizaos
