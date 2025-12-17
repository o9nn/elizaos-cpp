#include "muteRoom.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<bool> _shouldMute(State state) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto shouldMutePrompt = composePromptFromState({;
        state,
        template: shouldMuteTemplate, // Define this template separately
        });

        const auto response = runtime.useModel(ModelType.TEXT_SMALL, {;
            runtime,
            prompt: shouldMutePrompt,
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
                        thought: 'I will now mute this room',
                        actions: ['MUTE_ROOM_STARTED'],
                        },
                        metadata: {
                            type: 'MUTE_ROOM',
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
                                    thought: 'I decided to not mute this room',
                                    actions: ['MUTE_ROOM_FAILED'],
                                    },
                                    metadata: {
                                        type: 'MUTE_ROOM',
                                        },
                                        },
                                        'messages';
                                        );
                                    }

                                    // Default to false if response is unclear
                                    std::cout << "Unclear boolean response: " + std::to_string(response) << defaulting to false` << std::endl;
                                    return false;

}

} // namespace elizaos
