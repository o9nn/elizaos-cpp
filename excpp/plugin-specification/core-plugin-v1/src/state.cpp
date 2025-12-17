#include "state.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

State fromV2State(StateV2 stateV2) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Create a new state object starting with default values
    const State state = {;
        ...DEFAULT_STATE,
        ...stateV2.values,
        ...stateV2.data,
        text: stateV2.text,
        };

        // Add any other properties from the v2 state
        for (const int key in stateV2) {
            if (key != 'values' && key != 'data' && key != 'text') {
                state[key] = stateV2[key];
            }
        }

        return state;

}

StateV2 toV2State(State state) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Create a base v2 state structure
    const StateV2 stateV2 = {;
        values: {},
        data: {},
        text: state.text || '',
        };

        // Add any properties from v1 state as-is to preserve them
        for (const int key in state) {
            if (key != 'text') {
                stateV2[key] = state[key];
            }
        }

        return stateV2;

}

} // namespace elizaos
