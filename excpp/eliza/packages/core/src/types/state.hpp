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
 * Represents the current state or context of a conversation or agent interaction.
 * This interface is a flexible container for various pieces of information that define the agent's
 * understanding at a point in time. It includes:
 * - `values`: A key-value store for general state variables, often populated by providers.
 * - `data`: Another key-value store, potentially for more structured or internal data.
 * - `text`: A string representation of the current context, often a summary or concatenated history.
 * The `[key: string]: any;` allows for dynamic properties, though `EnhancedState` offers better typing.
 * This state object is passed to handlers for actions, evaluators, and providers.
 */
struct State {
    { values;
    { data;
    std::string text;
};


/**
 * Defines the possible primitive types or structured types for a value within the agent's state.
 * This type is used to provide more specific typing for properties within `StateObject` and `StateArray`,
 * moving away from a generic 'any' type for better type safety and clarity in state management.
 */
using StateValue = std::variant<std::string, double, bool, nullptr, StateObject, StateArray>;
/**
 * Represents a generic object structure within the agent's state, where keys are strings
 * and values can be any `StateValue`. This allows for nested objects within the state.
 * It's a fundamental part of the `EnhancedState` interface.
 */
struct StateObject {
};

/**
 * Represents an array of `StateValue` types within the agent's state.
 * This allows for lists of mixed or uniform types to be stored in the state,
 * contributing to the structured definition of `EnhancedState`.
 */
using StateArray = std::vector<StateValue>;

/**
 * Enhanced State interface with more specific types for its core properties.
 * This interface provides a more structured representation of an agent's conversational state,
 * building upon the base `State` by typing `values` and `data` as `StateObject`.
 * The `text` property typically holds a textual summary or context derived from the state.
 * Additional dynamic properties are still allowed via the index signature `[key: string]: StateValue;`.
 */
struct EnhancedState {
    StateObject values;
    StateObject data;
    std::string text;
};


} // namespace elizaos
