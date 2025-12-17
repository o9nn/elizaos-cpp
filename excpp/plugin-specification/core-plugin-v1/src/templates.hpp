#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "elizaos/core-plugin-v2.hpp"
#include "state.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Template type definition for v1 compatibility
 * A template can be either a string or a function that takes state and returns a string
 * This aligns with V2's TemplateType
 */
using TemplateType = std::function<void()>;

/**
 * Generic template values interface for typed access to state.values
 * Users can extend this interface for type safety in their templates
 */
struct TemplateValues {
};

/**
 * Create a template function from a v1 template
 * @param template The v1 template (string or function)
 * @returns A function that processes the template with the given state
 */
std::function<std::string(State)> createTemplateFunction(TemplateType template);

/**
 * Process a template with the given state
 * @param template The template to process (string or function)
 * @param state The state to use for processing
 * @returns The processed template string
 */
std::string processTemplate(TemplateType template, State state);

/**
 * Type-safe accessor for template values
 * @param state The state containing the values
 * @param defaultValues Optional default values to use if values are missing
 * @returns The values object with type information
 */

  // First cast state.values to a valid object type to use with spread

  // Create a new object with both default values and state values

} // namespace elizaos
