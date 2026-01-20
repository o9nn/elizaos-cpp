#pragma once
#include <algorithm>
#include <any>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use


/**
 * Example content with associated user for demonstration purposes
 */
struct ActionExample {
    std::string name;
    Content content;
};

/**
 * Callback function type for handlers
 */
using HandlerCallback = std::function<std::future<std::vector<Memory>>(Content, std::any)>;

/**
 * Handler function type for processing messages
 */
using Handler = std::function<std::future<std::any>(IAgentRuntime, Memory, State, std::any, HandlerCallback, std::vector<Memory>)>;

/**
 * Validator function type for actions/evaluators
 */
using Validator = std::function<std::future<bool>(IAgentRuntime, Memory, State)>;

/**
 * Represents an action the agent can perform
 */
struct Action {
    std::optional<std::vector<std::string>> similes;
    std::string description;
    std::optional<std::vector<std::vector<ActionExample>>> examples;
    Handler handler;
    std::string name;
    Validator validate;
};

/**
 * Example for evaluating agent behavior
 */
struct EvaluationExample {
    std::string prompt;
    std::vector<ActionExample> messages;
    std::string outcome;
};

/**
 * Evaluator for assessing agent responses
 */
struct Evaluator {
    std::optional<bool> alwaysRun;
    std::string description;
    std::optional<std::vector<std::string>> similes;
    std::vector<EvaluationExample> examples;
    Handler handler;
    std::string name;
    Validator validate;
};

struct ProviderResult {
    std::optional<std::string> text;
};

/**
 * Provider for external data/services
 */
struct Provider {
    std::string name;
    std::optional<std::string> description;
    std::optional<bool> dynamic;
    std::optional<double> position;
    std::optional<bool> private;
};


} // namespace elizaos
