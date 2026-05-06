#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "elizaos/core-plugin-v2.hpp"
#include "types.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Represents the state of a conversation or context
 * This is a v1 compatibility wrapper for v2 State
 */
using State = StateFromTypes;

/**
 * Default empty state with required properties
 */

/**
 * Converts v2 State to v1 compatible State
 * Uses the V2 State interface from core-plugin-v2
 */
State fromV2State(StateV2 stateV2);

/**
 * Converts v1 State to v2 State
 * Creates a state object conforming to V2 State interface
 */
StateV2 toV2State(State state);

} // namespace elizaos
