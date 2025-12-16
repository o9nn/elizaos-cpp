#include ".v2.hpp"
#include "types.hpp"
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

;
;

/**
 * Represents the state of a conversation or context
 * This is a v1 compatibility wrapper for v2 State
 */
using State = StateFromTypes;

/**
 * Default empty state with required properties
 */
const DEFAULT_STATE: Partial<State> = {
  bio: '',
  lore: '',
  messageDirections: '',
  postDirections: '',
  actors: '',
  recentMessages: '',
  recentMessagesData: [],
};

/**
 * Converts v2 State to v1 compatible State
 * Uses the V2 State interface from core-plugin-v2
 */
;

  // Add any other properties from the v2 state
  for (const key in stateV2) {
    if (key !== 'values' && key !== 'data' && key !== 'text') {
      state[key] = stateV2[key];
    }
  }

  return state;
}

/**
 * Converts v1 State to v2 State
 * Creates a state object conforming to V2 State interface
 */
,
    data: {},
    text: state.text || '',
  };

  // Add any properties from v1 state as-is to preserve them
  for (const key in state) {
    if (key !== 'text') {
      stateV2[key] = state[key];
    }
  }

  return stateV2;
}

} // namespace elizaos
