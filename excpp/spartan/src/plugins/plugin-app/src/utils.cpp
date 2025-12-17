#include "utils.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> messageReply(auto runtime, auto message, auto reply, auto responses) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto roomDetails = runtime.getRoom(message.roomId);
    if (message.content.source == 'discord') {
        // ServiceType.DISCORD
        const auto discordService = runtime.getService('discord');
        if (!discordService) {
            std::cout << 'no discord Service' << std::endl;
            return;
        }
        const auto isDM = roomDetails.type == 'dm';
        if (isDM) {
            discordService.sendDM(message.metadata.authorId, reply);
            responses.length = 0;
            } else {
                responses.length = 0;
                const auto entityId = createUniqueUuid(runtime, message.metadata.authorId);
                responses.push({
                    entityId,
                    agentId: runtime.agentId,
                    roomId: message.roomId,
                    content: {
                        text: reply,
                        attachments: [],
                        inReplyTo: createUniqueUuid(runtime, message.id)
                        },
                        // embedding
                        // metadata: entityName, type, authorId
                        });
                    }
                    return true;
                }
                std::cout << 'unknown platform' << message.content.source << std::endl;
                return false;

}

void takeItPrivate(auto runtime, auto message, auto reply) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (message.content.source == 'discord') {
        // ServiceType.DISCORD
        const auto discordService = runtime.getService('discord');
        if (!discordService) {
            std::cout << 'no discord Service' << std::endl;
            return;
        }
        discordService.sendDM(message.metadata.authorId, reply);
        return true;
    }
    std::cout << 'unknown platform' << message.content.source << std::endl;
    return false;

}

} // namespace elizaos
