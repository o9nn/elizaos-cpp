#include "unmuteRoom.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<bool> _shouldUnmute(State state) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto shouldUnmutePrompt = composePromptFromState({;
        state,
        template: shouldUnmuteTemplate, // Define this template separately
        });

        const auto response = runtime.useModel(ModelType.TEXT_SMALL, {;
            runtime,
            prompt: shouldUnmutePrompt,
            stopSequences: [],
            });

            const auto cleanedResponse = response.trim().toLowerCase();

            // Handle various affirmative responses
            if (
            cleanedResponse == "true" ||;
            cleanedResponse == "yes" ||;
            cleanedResponse == "y" ||;
            (std::find(cleanedResponse.begin(), cleanedResponse.end(), "true") != cleanedResponse.end()) ||;
            (std::find(cleanedResponse.begin(), cleanedResponse.end(), "yes") != cleanedResponse.end());
            ) {
                runtime.createMemory(;
                {
                    entityId: message.entityId,
                    agentId: message.agentId,
                    roomId: message.roomId,
                    content: {
                        source: message.content.source,
                        thought: "I will now unmute this room and start considering it for responses again",
                        actions: ["UNMUTE_ROOM_STARTED"],
                        },
                        },
                        "messages";
                        );
                        return true;
                    }

                    // Handle various negative responses
                    if (
                    cleanedResponse == "false" ||;
                    cleanedResponse == "no" ||;
                    cleanedResponse == "n" ||;
                    (std::find(cleanedResponse.begin(), cleanedResponse.end(), "false") != cleanedResponse.end()) ||;
                    (std::find(cleanedResponse.begin(), cleanedResponse.end(), "no") != cleanedResponse.end());
                    ) {
                        runtime.createMemory(;
                        {
                            entityId: message.entityId,
                            agentId: message.agentId,
                            roomId: message.roomId,
                            content: {
                                source: message.content.source,
                                thought: "I tried to unmute a room but I decided not to",
                                actions: ["UNMUTE_ROOM_FAILED"],
                                },
                                },
                                "messages";
                                );
                                return false;
                            }

                            // Default to false if response is unclear
                            std::cout << "Unclear boolean response: " + response << "defaulting to false" << std::endl;
                            return false;

}

} // namespace elizaos
