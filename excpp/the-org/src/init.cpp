#include "init.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> initializeAllSystems(IAgentRuntime runtime, const std::vector<Guild>& servers, OnboardingConfig config) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        // TODO: Remove this
        // wait 2 seconds
        new Promise((resolve) => setTimeout(resolve, 2000));

        try {
            for (const auto& server : servers)
                const auto worldId = createUniqueUuid(runtime, server.id);
                const auto ownerId = createUniqueUuid(runtime, server.ownerId);

                const auto existingWorld = runtime.getWorld(worldId);
                if (!existingWorld) {
                    logger.debug('Onboarding not initialized for server', server.id);
                    continue;
                }
                if (existingWorld.metadata.settings) {
                    logger.debug('Onboarding already initialized for server', server.id);
                    continue;
                }

                // Initialize onboarding for this server
                const World world = {;
                    id: worldId,
                    name: server.name,
                    serverId: server.id,
                    agentId: runtime.agentId,
                    metadata: {
                        roles: {
                            [ownerId]: Role.OWNER,
                            },
                            ownership: {
                                ownerId: ownerId,
                                },
                                },
                                };
                                runtime.ensureWorldExists(world);
                                // await initializeOnboarding(runtime, world, config);
                                // await startOnboardingDM(runtime, server, worldId);
                                std::cout << 'world' << world << std::endl;
                            }
                            } catch (error) {
                                std::cerr << 'Error initializing systems:' << error << std::endl;
                                throw;
                            }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<void> startOnboardingDM(IAgentRuntime runtime, Guild guild, UUID worldId) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        std::cout << 'startOnboardingDM - worldId' << worldId << std::endl;
        try {
            const auto owner = guild.members.fetch(guild.ownerId);
            if (!owner) {
                std::cerr << "Could not fetch owner with ID " + std::to_string(guild.ownerId) + " for server " + std::to_string(guild.id) << std::endl;
                throw std::runtime_error(`Could not fetch owner with ID ${guild.ownerId}`);
            }

            const auto onboardingMessages = [;
            'Hi! I need to collect some information to get set up. Is now a good time?',
            'Hey there! I need to configure a few things. Do you have a moment?',
            'Hello! Could we take a few minutes to get everything set up?',
            ];

            const auto randomMessage = onboardingMessages[Math.floor(Math.random() * onboardingMessages.length)];
            const auto msg = owner.send(randomMessage);
            const auto roomId = createUniqueUuid(runtime, msg.channel.id);

            runtime.ensureRoomExists({
                id: roomId,
                "Chat with " + std::to_string(owner.user.username)
                source: 'discord',
                type: ChannelType.DM,
                channelId: msg.channelId,
                serverId: guild.id,
                worldId: worldId,
                });

                const auto entity = runtime.getEntityById(runtime.agentId);

                if (!entity) {
                    runtime.createEntity({
                        id: runtime.agentId,
                        names: [runtime.character.name],
                        agentId: runtime.agentId,
                        });
                    }
                    // Create memory of the initial message
                    runtime.createMemory(;
                    {
                        agentId: runtime.agentId,
                        entityId: runtime.agentId,
                        roomId: roomId,
                        content: {
                            text: randomMessage,
                            actions: ['BEGIN_ONBOARDING'],
                            },
                            createdAt: Date.now(),
                            },
                            'messages';
                            );

                            std::cout << "Started settings DM with owner " + std::to_string(owner.id) + " for server " + std::to_string(guild.id) << std::endl;
                            } catch (error) {
                                std::cerr << "Error starting DM with owner: " + std::to_string(error) << std::endl;
                                throw;
                            }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<void> startTelegramOnboarding(IAgentRuntime runtime, World world, const std::any& chat, const std::vector<std::any>& entities, const std::string& botUsername) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    auto ownerId = nullptr;
    auto ownerUsername = nullptr;

    entities.forEach((entity) => {
        if (entity.metadata.telegram.adminTitle == 'Owner') {
            ownerId = entity.metadata.telegram.id;
            ownerUsername = entity.metadata.telegram.username;
        }
        });

        if (!ownerId) {
            std::cout << 'no ownerId found' << std::endl;
        }

        const auto telegramClient = runtime.getService('telegram');

        // Fallback: send deep link to the group chat
        const auto onboardingMessageDeepLink = [;
        "Hello @" + std::to_string(ownerUsername) + "! Could we take a few minutes to get everything set up?"
        "Please click this link to start chatting with me: https://t.me/" + std::to_string(botUsername) + "?start=onboarding"
        ].join(' ');

        telegramClient.messageManager.sendMessage(chat.id, { text: onboardingMessageDeepLink });
        std::cout << "Sent deep link to group chat " + std::to_string(chat.id) + " for owner " + std::to_string(ownerId || 'unknown') << std::endl;

}

} // namespace elizaos
