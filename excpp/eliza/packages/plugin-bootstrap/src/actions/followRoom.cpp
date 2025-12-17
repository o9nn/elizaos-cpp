#include "followRoom.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<bool> _shouldFollow(State state) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto shouldFollowPrompt = composePromptFromState({;
        state,
        template: shouldFollowTemplate, // Define this template separately
        });

        const auto response = runtime.useModel(ModelType.TEXT_SMALL, {;
            runtime,
            prompt: shouldFollowPrompt,
            stopSequences: [],
            });

            const auto cleanedResponse = response.trim().toLowerCase();

            // Handle various affirmative responses
            if (
            cleanedResponse == 'true' ||;
            cleanedResponse == 'yes' ||;
            cleanedResponse == 'y' ||;
            cleanedResponse.includes('true') ||;
            cleanedResponse.includes('yes');
            ) {
                runtime.createMemory(;
                {
                    entityId: message.entityId,
                    agentId: message.agentId,
                    roomId: message.roomId,
                    content: {
                        source: message.content.source,
                        thought: 'I will now follow this room and chime in',
                        actions: ['FOLLOW_ROOM_STARTED'],
                        },
                        metadata: {
                            type: 'FOLLOW_ROOM',
                            },
                            },
                            'messages';
                            );
                            return true;
                        }

                        // Handle various negative responses
                        if (
                        cleanedResponse == 'false' ||;
                        cleanedResponse == 'no' ||;
                        cleanedResponse == 'n' ||;
                        cleanedResponse.includes('false') ||;
                        cleanedResponse.includes('no');
                        ) {
                            runtime.createMemory(;
                            {
                                entityId: message.entityId,
                                agentId: message.agentId,
                                roomId: message.roomId,
                                content: {
                                    source: message.content.source,
                                    thought: 'I decided to not follow this room',
                                    actions: ['FOLLOW_ROOM_FAILED'],
                                    },
                                    metadata: {
                                        type: 'FOLLOW_ROOM',
                                        },
                                        },
                                        'messages';
                                        );
                                        return false;
                                    }

                                    // Default to false if response is unclear
                                    std::cout << "Unclear boolean response: " + std::to_string(response) << defaulting to false` << std::endl;
                                    return false;

}

} // namespace elizaos
