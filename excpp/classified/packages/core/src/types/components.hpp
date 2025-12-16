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
 * Example content with associated user for demonstration purposes
 */
struct ActionExample {
    std::string name;
    Content content;
};

/**
 * Callback function type for handlers
 */
using HandlerCallback = (response: Content, files?: any) => Promise<Memory[]>;

/**
 * Handler function type for processing messages
 */
using Handler = std::variant<(

/**
 * Validator function type for actions/evaluators
 */
using Validator = (

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
    std::optional<{> values;
    std::optional<{> data;
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
    (runtime: IAgentRuntime, message: Memory, state: State) => Promise<ProviderResult> get;
};

/**
 * Result returned by an action after execution
 * Used for action chaining and state management
 */
struct ActionResult {
    std::optional<std::string> text;
    std::optional<std::unordered_map<std::string, std::any>> values;
    std::optional<std::unordered_map<std::string, std::any>> data;
    bool success;
    std::optional<string | Error> error;
};

/**
 * Context provided to actions during execution
 * Allows actions to access previous results and update state
 */
struct ActionContext {
    std::vector<ActionResult> previousResults;
    std::optional<(actionName: string) => ActionResult | undefined> getPreviousResult;
};

/**
 * Helper function to create ActionResult with proper defaults
 */
ActionResult createActionResult(const std::optional<ActionResult>& partial);

} // namespace elizaos
