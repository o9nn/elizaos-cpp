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
 * Common types for agent modules
 */

/**
 * Model response output
 */
struct ModelOutput {
    std: message;
    std::optional<std:> content;
    std::optional<std::vector<ToolCall>> toolCalls;
    std::optional<std::vector<ThinkingBlock>> thinkingBlocks;
    std::optional<{> usage;
    double promptTokens;
    double completionTokens;
    double totalTokens;
};

/**
 * Tool call structure
 */
struct ToolCall {
    std: id;
    'function' type;
    { std::function;
    std: name;
    std: arguments;
};

/**
 * Thinking block structure
 */
struct ThinkingBlock {
    std: type;
    std: content;
};

/**
 * Command definition
 */
struct Command {
    std: name;
    std::optional<std:> endName;
    std::optional<std:> installScript;
};

/**
 * Format dictionary for template rendering
 */
struct FormatDict {
};

/**
 * Extra fields for problem statements and other configs
 */
struct ExtraFields {
};

/**
 * Stats data structure
 */
struct StatsData {
    double instanceCost;
    double tokensSent;
    double tokensReceived;
    double apiCalls;
};

/**
 * API response structure
 */
struct APIResponse {
    std::optional<Array<{> choices;
    std::optional<{> message;
    std: content;
    std: role;
    double index;
    std::optional<{> usage;
    std::optional<double> prompt_tokens;
    std::optional<double> completion_tokens;
    std::optional<double> total_tokens;
};

/**
 * History message content type
 */
using MessageContent = std::variant<, std:, Array<{
      type: 'text', 'image_url'>;
      text?: std:;
      image_url?: { url: std: };
      cacheControl?: { type: std: };
    }>;

} // namespace elizaos
