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

import type { Memory } from './memory';
import type { Content } from './primitives';
import type { IAgentRuntime } from './runtime';
import type { State } from './state';

/**
 * Example content with associated user for demonstration purposes
 */
struct ActionExample {
    std::string name;
    Content content;
};


/**
 * Callback ,
  callback?: HandlerCallback,
  responses?: Memory[]
) => Promise<unknown>;

/**
 * Validator ;


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


} // namespace elizaos
