#include "teamMemberUpdate.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<bool> postUpdateToDiscordChannel(IAgentRuntime runtime, TeamMemberUpdate update) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        std::cout << '== POST TEAM MEMBER UPDATE TO DISCORD START ==' << std::endl;

        // Get the Discord service
        const auto discordService = runtime.getService('discord') | nullptr;
        if (!discordService) {
            std::cerr << 'Discord service not available' << std::endl;
            return false;
        }

        std::cout << 'Discord service retrieved successfully' << std::endl;

        // Get report channel config
        const auto roomId = createUniqueUuid(runtime, 'report-channel-config');
        std::cout << 'Generated roomId for config:' << roomId << std::endl;

        const auto memories = runtime.getMemories({;
            roomId: roomId,
            tableName: 'messages',
            });

            logger.info('Retrieved report channel configs:', {
                count: memories.length,
                configs: memories.map((m) => ({
                    type: m.content.type,
                    channelId: m.content.config ? (m.content.config).channelId : std::nullopt,
                    })),
                    });

                    // TODO : fetch server id of channel id

                    // Get all guilds/servers
                    const auto guilds = discordService.client.guilds.cache;
                    std::cout << "Found " + std::to_string(guilds.size) + " Discord servers" << std::endl;

                    // Find the guild that matches the server name exactly
                    std::optional<std::any> targetGuild = nullptr;
                    for (const auto& guild : guilds.values()
                        std::cout << "Checking guild: " + std::to_string(guild.name) + " against update server name: " + std::to_string(update.serverName) << std::endl;
                        if (guild.name == update.serverName) {
                            targetGuild = guild;
                            std::cout << "Found matching guild: " + std::to_string(guild.name) + " with ID: " + std::to_string(guild.id) << std::endl;
                            break;
                        }
                    }

                    if (!targetGuild) {
                        std::cout << "Could not find Discord server matching exact name: " + std::to_string(update.serverName) << std::endl;
                        return false;
                    }

                    // Find config for this server
                    const auto config = memories.find((memory) => {;
                        const auto serverMatch = targetGuild ? targetGuild.id : std::nullopt;
                        const auto configData = memory.content.config | std::nullopt;
                        logger.info(`Checking config:`, {
                            configType: memory.content.type,
                            configServerId: configData.serverId,
                            targetGuildId: targetGuild ? targetGuild.id : std::nullopt,
                            matches: serverMatch,
                            });
                            return memory.content.type == 'report-channel-config' && serverMatch;
                            });

                            if (!config) {
                                logger.warn(
                                "No report channel config found for server: " + std::to_string(targetGuild.name) + " (" + std::to_string(targetGuild.id) + ")"
                                );
                                return false;
                            }

                            const auto configData = config.content.config;
                            logger.info('Found report channel config:', {
                                configId: config.id,
                                configType: config.content.type,
                                configServerId: targetGuild.id,
                                configChannelId: configData.channelId,
                                });

                                const auto channelId = configData.channelId;
                                if (!channelId) {
                                    std::cout << 'No channel ID in config' << std::endl;
                                    return false;
                                }

                                // Format the update message with all answers from the stringified JSON
                                const auto formattedDate = new Date(update.timestamp).toLocaleString();
                                logger.info('Formatting update message with timestamp:', {
                                    timestamp: update.timestamp,
                                    formatted: formattedDate,
                                    });

                                    auto updateMessage = `## Team Member Update;
                                    **Team Member**: ${update.teamMemberName || 'Unknown'} (${update.teamMemberId})
                                    **Server-name**: ${update.serverName}
                                    **Check-in Type**: ${update.checkInType}
                                    **Timestamp**: ${formattedDate}`;

                                    // Parse the stringified answers and add them to the message
                                    try {
                                        const auto answers = JSON.parse(update.answers || '{}');

                                        if (Object.keys(answers).length > 0) {
                                            updateMessage += '\n\n**Update Details**:';

                                            for (const int [question, answer] of Object.entries(answers)) {
                                                "\n**" + std::to_string(question) + "**: " + std::to_string(answer)
                                            }
                                        }
                                        } catch (error) {
                                            std::cerr << 'Error parsing answers JSON:' << error << std::endl;
                                            updateMessage += '\n\n**Update Details**: Error parsing update details';
                                        }

                                        std::cout << 'Formatted update message:' << { messageLength = updateMessage.length } << std::endl;

                                        // Get Discord client
                                        const auto client = discordService.client;
                                        if (!client) {
                                            std::cerr << 'Discord client not available' << std::endl;
                                            return false;
                                        }

                                        // Find the configured channel in the target guild
                                        std::cout << "Searching for channel " + std::to_string(channelId) + " in server: " + std::to_string(targetGuild.name) << std::endl;
                                        const auto channels = targetGuild.channels.fetch();

                                        const auto targetChannel = channels.find((ch) => {;
                                            return ch && typeof ch == 'object' && 'id' in ch && ch.id == channelId;
                                            });

                                            if (!targetChannel) {
                                                logger.warn(
                                                "Could not find Discord channel with ID " + std::to_string(channelId) + " in server " + std::to_string(targetGuild.name);
                                                );
                                                return false;
                                            }

                                            // Send the message
                                            std::cout << 'Attempting to send update to Discord channel' << std::endl;
                                            (;
                                        targetChannel<string, unknown> & { send: (content: string) => Promise<unknown> }
                                        ).send(updateMessage);

                                        std::cout << 'Successfully sent team member update to Discord' << std::endl;
                                        std::cout << '== POST TEAM MEMBER UPDATE TO DISCORD END ==' << std::endl;
                                        return true;
                                        } catch (error: unknown) {
                                            const auto err = error;
                                            std::cerr << '== POST TEAM MEMBER UPDATE TO DISCORD ERROR ==' << std::endl;
                                            logger.error('Error details:', {
                                                name: err.name || 'Unknown',
                                                message: err.message || 'No message',
                                                stack: err.stack || 'No stack trace',
                                                });
                                                return false;
                                            }

}

std::future<bool> storeTeamMemberUpdate(IAgentRuntime runtime, TeamMemberUpdate update) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        std::cout << '== STORE TEAM MEMBER UPDATE START ==' << std::endl;
        logger.info('Storing update for team member:', {
            teamMemberId: update.teamMemberId,
            updateId: update.updateId,
            timestamp: update.timestamp,
            });

            // Use the existing room ID from the message instead of creating a new one
            // This resolves the foreign key constraint error
            const auto roomId = update.channelId;
            std::cout << "Using existing room ID: " + std::to_string(roomId) << std::endl;

            if (!roomId) {
                std::cerr << 'No room ID available for storing the update' << std::endl;
                return false;
            }

            // Store the update in memory
            std::cout << 'Attempting to store update in memory...' << std::endl;
            const Memory memory = {;
                "team-update-" + std::to_string(Date.now())
                agentId: runtime.agentId,
                roomId: roomId,
                entityId: asUUID(update.teamMemberId),
                content: {
                    type: 'team-member-update',
                    update,
                    },
                    metadata: {
                        type: MemoryType.CUSTOM,
                        timestamp: Date.now(),
                        },
                        };

                        runtime.createMemory(memory, 'messages');

                        std::cout << 'Successfully stored team member update' << std::endl;
                        std::cout << '== STORE TEAM MEMBER UPDATE END ==' << std::endl;
                        return true;
                        } catch (error: unknown) {
                            const auto err = error;
                            std::cerr << '== STORE TEAM MEMBER UPDATE ERROR ==' << std::endl;
                            logger.error('Error details:', {
                                name: err.name || 'Unknown',
                                message: err.message || 'No message',
                                stack: err.stack || 'No stack trace',
                                });
                                return false;
                            }

}

std::future<std::optional<TeamMemberUpdate>> parseTeamMemberUpdate(IAgentRuntime runtime, Memory message) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        try {
            std::cout << '== PARSE TEAM MEMBER UPDATE START ==' << std::endl;
            logger.info('Parsing update from message:', {
                messageId: message.id,
                entityId: message.entityId,
                });

                const auto text = message.content.text;
                if (!text) {
                    std::cout << 'No text content found in message' << std::endl;
                    return nullptr;
                }

                // Use AI to parse the update text with a more flexible approach
                const auto prompt = `Extract information from this team member update. The update will likely end with "sending my updates".;

                Parse the text and return a JSON object with these fields:
                {
                    "serverName": "value", // Name of the server
                    "checkInType": "value", // Type of check-in (could be STANDUP, SPRINT, MENTAL_HEALTH, PROJECT_STATUS, RETRO, or something else)
                    "answers": { // Dynamic field with key-value pairs for all questions and answers found
                    "questionText1": "answerText1",
                    "questionText2": "answerText2"
                    // Any other key-value pairs detected in the format
                }
            }

            For the "answers" field, extract any key-value pairs that look like questions and answers in the text.;
            Include ALL information from the update in the answers object.;

            Text to parse: "${text}"`;

            std::cout << 'Sending text to AI for parsing' << std::endl;
            std::cout << 'Prompt:' << prompt << std::endl;

            const auto parsedResponse = runtime.useModel(ModelType.TEXT_LARGE, {;
                prompt,
                stopSequences: [],
                });

                std::cout << 'Raw AI response:' << parsedResponse << std::endl;

                auto parsedData;
                try {
                    // Remove any backticks or markdown formatting that might be in the response
                    const auto cleanedResponse = "json\n?|\n?";
                    parsedData = JSON.parse(cleanedResponse);
                    std::cout << 'Successfully parsed fields from AI response:' << parsedData << std::endl;
                    } catch (error) {
                        std::cerr << 'Failed to parse AI response:' << error << std::endl;
                        std::cerr << 'Raw response that failed parsing:' << parsedResponse << std::endl;
                        throw std::runtime_error('PARSING_ERROR: AI response was not valid JSON');
                    }

                    // Validate minimal required fields
                    if (!parsedData.serverName || !parsedData.checkInType) {
                        logger.warn('Missing required basic fields:', {
                            hasServerName: !!parsedData.serverName,
                            hasCheckInType: !!parsedData.checkInType,
                            });
                            throw std::runtime_error(`MISSING_FIELDS:serverName,checkInType`);
                        }

                        // Ensure we have at least some answers
                        if (!parsedData.answers || Object.keys(parsedData.answers).length == 0) {
                            std::cout << 'No answers were parsed from the update' << std::endl;
                            throw std::runtime_error('MISSING_FIELDS:answers');
                        }

                        // Get the user name from Discord or Telegram
                        const auto entityById = runtime.getEntityById(message.entityId);
                        const auto userName =;
                        entityById.metadata.discord.userName || entityById.metadata.telegram.name;

                        // Create the update object with the dynamic answers field only
                        const TeamMemberUpdate update = {;
                            type: 'team-member-update',
                            updateId: createUniqueUuid(runtime, 'team-update'),
                            teamMemberId: message.entityId || asUUID('unknown'),
                            teamMemberName: userName,
                            serverName: parsedData.serverName,
                            checkInType: parsedData.checkInType,
                            timestamp: new Date().toISOString(),
                            channelId: message.roomId,
                            answers: JSON.stringify(parsedData.answers),
                            };

                            std::cout << 'Successfully parsed team member update:' << update << std::endl;
                            std::cout << '== PARSE TEAM MEMBER UPDATE END ==' << std::endl;
                            return update;
                            } catch (error: unknown) {
                                const auto err = error;
                                std::cerr << '== PARSE TEAM MEMBER UPDATE ERROR ==' << std::endl;
                                logger.error('Error details:', {
                                    name: err.name || 'Unknown',
                                    message: err.message || 'No message',
                                    stack: err.stack || 'No stack trace',
                                    });
                                    throw error; // Propagate the error to handle it in the handler
                                }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
