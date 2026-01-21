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

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use


/**
 * Represents an action called 'IGNORE'.
 *
 * This action is used to ignore the user in a conversation. It should be used when the user is aggressive, creepy, or when the conversation has naturally ended.
 * Avoid using this action if the user has engaged directly or if there is a need to communicate with them. Use IGNORE only when the user should be ignored.
 *
 * The action includes a validation function that always returns true and a handler function that also returns true.
 *
 * Examples of using the IGNORE action are provided in the 'examples' array. Each example includes messages between two parties and the use of the IGNORE action.
 *
 * @typedef {Action} ignoreAction
 */
    // If a callback and the agent's response content are available, call the callback
      // Pass the agent's original response content (thought, IGNORE action, etc.)
    // Still return true to indicate the action handler succeeded


} // namespace elizaos
