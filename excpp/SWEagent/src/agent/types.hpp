#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>

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
    std::string message;
    std::optional<std::string> content;
    std::optional<std::vector<ToolCall>> toolCalls;
    std::optional<std::vector<ThinkingBlock>> thinkingBlocks;
    double promptTokens;
    double completionTokens;
    double totalTokens;
};

/**
 * Tool call structure
 */
struct ToolCall {
    std::string id;
    std::string type;
    std::string name;
    std::string arguments;
};

/**
 * Thinking block structure
 */
struct ThinkingBlock {
    std::string type;
    std::string content;
};

/**
 * Command definition
 */
struct Command {
    std::string name;
    std::optional<std::string> endName;
    std::optional<std::string> installScript;
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
    std::string content;
    std::string role;
    double index;
    std::optional<double> prompt_tokens;
    std::optional<double> completion_tokens;
    std::optional<double> total_tokens;
};

/**
 * History message content type
 */
using MessageContent = std::variant<, std::string, Array<{

} // namespace elizaos
