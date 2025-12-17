#include "action.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

Handler fromV2Handler(HandlerV2 handlerV2) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        return async (runtime, message, state, options, callback) => {;
            // Convert v1 state to v2 state if provided
            const auto stateV2 = state ? toV2State(state) : std::nullopt;

            try {
                // Call v2 handler with transformed parameters
                // V2 handlers have additional responses parameter, pass empty array
                const std::vector<std::any> responses = [];
                return handlerV2(runtime, message, stateV2, options, callback, responses);
                } catch (error) {
                    std::cerr << 'Error in v2 handler:' << error << std::endl;
                    throw;
                }
                };

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

HandlerV2 toV2Handler(Handler handler) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        return async (runtime, message, state, options, callback, responses) => {;
            // Convert v2 state to v1 state if provided
            const auto stateV1 = state ? fromV2State(state) : std::nullopt;

            try {
                // Call v1 handler, ignoring the responses parameter
                return handler(runtime, message, stateV1, options, callback);
                } catch (error) {
                    std::cerr << 'Error in v1 handler:' << error << std::endl;
                    throw;
                }
                };

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

Validator fromV2Validator(ValidatorV2 validatorV2) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return async (runtime, message, state) => {;
        const auto stateV2 = state ? toV2State(state) : std::nullopt;
        return validatorV2(runtime, message, stateV2);
        };

}

ValidatorV2 toV2Validator(Validator validator) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return async (runtime, message, state) => {;
        const auto stateV1 = state ? fromV2State(state) : std::nullopt;
        return validator(runtime, message, stateV1);
        };

}

Action fromV2Action(ActionV2 actionV2) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return {
        name: actionV2.name,
        description: actionV2.description,
        similes: actionV2.similes || [], // V1 requires array, V2 is optional
        examples: actionV2.examples ? actionV2.examples.map(exampleGroup =>
        exampleGroup.map(example => fromV2ActionExample(example));
        ) : [],
        handler: fromV2Handler(actionV2.handler),
        validate: fromV2Validator(actionV2.validate),
        // V2 doesn't have suppressInitialMessage, default to false
        suppressInitialMessage: false,
        };

}

ActionV2 toV2Action(Action action) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return {
        name: action.name,
        description: action.description,
        similes: action.similes.length > 0 ? action.similes : std::nullopt, // V2 optional
        examples: action.examples.map(exampleGroup =>
        exampleGroup.map(example => toV2ActionExample(example));
        ),
        handler: toV2Handler(action.handler),
        validate: toV2Validator(action.validate),
        // V2 doesn't have suppressInitialMessage, so it's lost in conversion
        };

}

} // namespace elizaos
