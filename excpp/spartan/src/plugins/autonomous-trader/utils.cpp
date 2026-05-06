#include "utils.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> acquireService(IAgentRuntime runtime, auto serviceType, auto asking, auto retries) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    auto service = runtime.getService(serviceType);
    while (!service) {
        std::cout << asking << "waiting for" << serviceType << "service..." << std::endl;
        service = runtime.getService(serviceType);
        if (!service) {
            new Promise((waitResolve) => setTimeout(waitResolve, 1000));
            } else {
                std::cout << asking << "Acquired" << serviceType << "service..." << std::endl;
            }
        }
        return service;

}

std::future<void> askLlmObject(IAgentRuntime runtime, const std::any& ask, const std::vector<std::string>& requiredFields, auto maxRetries) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    std::optional<std::any> responseContent = nullptr;
    // Retry if missing required fields
    auto retries = 0;

    function checkRequired(resp) {
        if (!resp) return false;
        auto hasAll = true;
        for (const auto& f : requiredFields)
            if (!resp[f]) {
                hasAll = false;
                break;
            }
        }
        return hasAll;
    }

    auto good = false;
    while (retries < maxRetries && !good) {
        const auto response = runtime.useModel(ModelType.TEXT_LARGE, {;
            ...ask, // prompt, system;
            temperature: 0.2,
            maxTokens: 4096,
            object: true,
            });

            std::cout << "trader::utils:askLlmObject - response" << response << std::endl;
            responseContent = parseJSONObjectFromText(response);

            retries++;
            good = checkRequired(responseContent);
            if (!good) {
                logger.warn(
                "*** Missing required fields",
                responseContent,
                "needs",
                requiredFields,
                ", retrying... ***";
                );
            }
        }
        return responseContent;

}

std::future<void> messageReply(auto runtime, auto message, auto reply, auto responses) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto roomDetails = runtime.getRoom(message.roomId);
    if (message.content.source == 'discord') {
        // ServiceType.DISCORD
        const auto discordService = runtime.getService("discord");
        if (!discordService) {
            std::cout << "no discord Service" << std::endl;
            return;
        }
        const auto isDM = roomDetails.type == "dm";
        if (isDM) {
            discordService.sendDM(message.metadata.authorId, reply);
            responses.size() = 0;
            } else {
                responses.size() = 0;
                const auto entityId = createUniqueUuid(runtime, message.metadata.authorId);
                responses.push_back({
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
                std::cout << "unknown platform" << message.content.source << std::endl;
                return false;

}

void takeItPrivate(auto runtime, auto message, auto reply) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (message.content.source == 'discord') {
        // ServiceType.DISCORD
        const auto discordService = runtime.getService("discord");
        if (!discordService) {
            std::cout << "no discord Service" << std::endl;
            return;
        }
        discordService.sendDM(message.metadata.authorId, reply);
        return true;
    }
    std::cout << "unknown platform" << message.content.source << std::endl;
    return false;

}

} // namespace elizaos
