#include "unfollowRoom.hpp"
#include <future>
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<bool> _shouldUnfollow(State state) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto shouldUnfollowPrompt = composePromptFromState({;
        state,
        template: shouldUnfollowTemplate, // Define this template separately
        });

        const auto response = runtime.useModel(ModelType.TEXT_SMALL, {;
            prompt: shouldUnfollowPrompt,
            });

            const auto parsedResponse = parseBooleanFromText(response);

            return parsedResponse;

}

} // namespace elizaos
