#include "elizaos/core.hpp"
#include "index.hpp"
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



// Claude API Types
struct ClaudeTool {
    std::string name;
    std::optional<std::string> description;
    std::any input_schema;
};

using ClaudeContent = std::variant<, std::string, Array<{

struct ClaudeMessage {
    'user' | 'assistant' role;
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
    std::optional<{ type: 'auto' | 'any' | 'tool'; name?: string }> tool_choice;

struct ClaudeResponse {
    std::string id;
    'message' type;
    'assistant' role;
    std::string model;
    Array<{ content;
    'text' | 'tool_use' type;
    std::optional<std::string> text;
    std::optional<std::string> id;
    std::optional<std::string> name;
    std::optional<std::any> input;
    std::string stop_reason;
    { usage;
    double input_tokens;
    double output_tokens;
};

// OpenAI-style types for compatibility
struct OpenAIMessage {
    'system' | 'user' | 'assistant' | 'tool' role;
    std::optional<Array<{> tool_calls;
    std::string id;
    'function' type;
    { function;
    std::string name;
    std::string arguments;
    std::optional<std::string> tool_call_id;
};

/**
 * Claude-to-Provider Proxy
 * Converts Claude API requests to the appropriate provider format
 */
class ClaudeProxy {
  constructor(private runtime: IAgentRuntime) {}

  /**
   * Process a Claude API request and route it to the appropriate provider
   */
    // Determine which provider to use

        // Native Anthropic - make direct API request

  /**
   * Make a direct proxy request to Anthropic's API
   */

      // Make direct request to Anthropic API

  /**
   * Select the best available provider
   */
    // Check available providers in order of preference

  /**
   * Check if a provider is available
   */
        // Ollama is always available if installed

  /**
   * Handle OpenAI requests by converting format
   */

    // Create a formatted prompt for the runtime

  /**
   * Handle Groq requests
   */
    // Groq uses OpenAI-compatible format

  /**
   * Handle Ollama requests
   */
    // Ollama also uses OpenAI-compatible format

  /**
   * Convert Claude messages to OpenAI format
   */

  /**
   * Convert Claude messages to a simple prompt string
   */

  /**
   * Format OpenAI messages as a prompt
   */

  /**
   * Format response in Claude format
   */

/**
 * Create a Claude API handler for the inference plugin
 */
void createClaudeHandler(IAgentRuntime runtime);

} // namespace elizaos
