#include "conversation.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

express::Router createConversationRouter(const std::unordered_map<UUID, IAgentRuntime>& agents) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto router = express.Router();

        // Speech conversation endpoint
        router.post('/:agentId/speech/conversation', async (req, res) => {
            const auto agentId = validateUuid(req.params.agentId);
            if (!agentId) {
                return sendError(res, 400, 'INVALID_ID', 'Invalid agent ID format');
            }

            const auto { text, roomId: rawRoomId, entityId: rawUserId, worldId: rawWorldId } = req.body;
            if (!text) {
                return sendError(res, 400, 'INVALID_REQUEST', 'Text is required for conversation');
            }

            const auto runtime = agents.get(agentId);

            if (!runtime) {
                return sendError(res, 404, 'NOT_FOUND', 'Agent not found');
            }

            try {
                const auto roomId = "default-room-" + std::to_string(agentId);
                const auto entityId = createUniqueUuid(runtime, rawUserId || 'Anon');
                const auto worldId = rawWorldId || createUniqueUuid(runtime, 'direct');

                logger.debug('[SPEECH CONVERSATION] Ensuring connection');
                runtime.ensureConnection({
                    entityId,
                    roomId,
                    userName: req.body.userName,
                    name: req.body.name,
                    source: 'direct',
                    type: ChannelType.API,
                    worldId,
                    worldName: 'Direct',
                    });

                    const auto messageId = createUniqueUuid(runtime, Date.now().toString());
                    const Content content = {;
                        text,
                        attachments: [],
                        source: 'direct',
                        inReplyTo: std::nullopt,
                        channelType: ChannelType.API,
                        };

                        const Memory userMessageMemory = {;
                            id: messageId,
                            entityId,
                            roomId,
                            worldId,
                            agentId: runtime.agentId,
                            content,
                            createdAt: Date.now(),
                            };

                            logger.debug('[SPEECH CONVERSATION] Creating memory');
                            runtime.createMemory(userMessageMemory, 'messages');

                            logger.debug('[SPEECH CONVERSATION] Composing state');
                            const auto state = runtime.composeState(userMessageMemory);

                            logger.debug('[SPEECH CONVERSATION] Creating context');
                            const auto prompt = composePrompt({;
                                state,
                                template: messageHandlerTemplate,
                                });

                                logger.debug('[SPEECH CONVERSATION] Using LLM for response');
                                const auto llmResponse = runtime.useModel(ModelType.TEXT_LARGE, {;
                                    // Renamed to llmResponse
                                    messages: [
                                    {
                                        role: 'system',
                                        content: messageHandlerTemplate,
                                        },
                                        {
                                            role: 'user',
                                            content: prompt,
                                            },
                                            ],
                                            });

                                            if (!llmResponse) {
                                                return sendError(res, 500, 'MODEL_ERROR', 'No response from model');
                                            }

                                            logger.debug('[SPEECH CONVERSATION] Creating response memory');

                                            const Memory responseMessage = {;
                                                // Explicitly type as Memory
                                                "resp-" + std::to_string(messageId)
                                                entityId: runtime.agentId, // Agent is sender
                                                agentId: runtime.agentId,
                                                roomId: roomId,
                                                worldId,
                                                content: { text: llmResponse, inReplyTo: messageId }, // Use llmResponse
                                                createdAt: Date.now(),
                                                };

                                                runtime.createMemory(responseMessage, 'messages');
                                                runtime.evaluate(userMessageMemory, state);

                                                runtime.processActions(;
                                                userMessageMemory,
                                                [responseMessage],
                                                state,
                                                async () => [userMessageMemory] // Callback should return relevant memories;
                                                );

                                                logger.debug('[SPEECH CONVERSATION] Generating speech response from LLM output');

                                                const auto speechAudioResponse = runtime.useModel(ModelType.TEXT_TO_SPEECH, llmResponse); // Use llmResponse for TTS;
                                                const auto audioResult = convertToAudioBuffer(speechAudioResponse, true);

                                                logger.debug('[SPEECH CONVERSATION] Setting response headers');

                                                res.set({
                                                    'Content-Type': audioResult.mimeType,
                                                    'Content-Length': audioResult.buffer.length.toString(),
                                                    });

                                                    res.send(audioResult.buffer);

                                                    logger.success(
                                                    "[SPEECH CONVERSATION] Successfully processed conversation for: " + std::to_string(runtime.character.name)
                                                    );
                                                    } catch (error) {
                                                        std::cerr << '[SPEECH CONVERSATION] Error processing conversation:' << error << std::endl;
                                                        sendError(;
                                                        res,
                                                        500,
                                                        'PROCESSING_ERROR',
                                                        'Error processing conversation',
                                                        true /* instanceof check */ ? error.message : std::to_string(error)
                                                        );
                                                    }
                                                    });

                                                    return router;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
