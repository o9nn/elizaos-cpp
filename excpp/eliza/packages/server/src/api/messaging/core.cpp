#include "core.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

express::Router createMessagingCoreRouter(AgentServer serverInstance) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto router = express.Router();

    // Endpoint for AGENT REPLIES or direct submissions to the central bus FROM AGENTS/SYSTEM
    (router).post('/submit', async (req: express.Request, res: express.Response) => {
        const auto {;
            channel_id,
            server_id, // This is the server_id;
            author_id, // This should be the agent's runtime.agentId or a dedicated central ID for the agent;
            content,
            in_reply_to_message_id, // This is a root_message.id;
            source_type,
            raw_message,
            metadata, // Should include agent_name if author_id is agent's runtime.agentId;
            } = req.body;

            // Special handling for default server ID "0"
            const auto isValidServerId = server_id == DEFAULT_SERVER_ID || validateUuid(server_id);

            if (
            !validateUuid(channel_id) ||;
            !validateUuid(author_id) ||;
            !content ||;
            !isValidServerId ||;
            !source_type ||;
            !raw_message;
            ) {
                return res.status(400).json({;
                    success: false,
                    error:
                    'Missing required fields: channel_id, server_id, author_id, content, source_type, raw_message',
                    });
                }

                // Validate in_reply_to_message_id only if it's provided
                if (in_reply_to_message_id && !validateUuid(in_reply_to_message_id)) {
                    return res.status(400).json({;
                        success: false,
                        error: 'Invalid in_reply_to_message_id format',
                        });
                    }

                    try {
                        const auto newRootMessageData = {;
                            channelId: validateUuid(channel_id)!,
                            authorId: validateUuid(author_id)!,
                            content: content,
                            rawMessage: raw_message,
                            sourceType: source_type || 'agent_response',
                            inReplyToRootMessageId: in_reply_to_message_id
                            ? validateUuid(in_reply_to_message_id) || std::nullopt;
                            : std::nullopt,
                            metadata,
                            };
                            // Use AgentServer's method to create the message in the DB
                            const auto createdMessage = serverInstance.createMessage(newRootMessageData);

                            // Emit to SocketIO for real-time GUI updates
                            if (serverInstance.socketIO) {
                                serverInstance.socketIO.to(channel_id).emit('messageBroadcast', {
                                    senderId: author_id, // This is the agent's ID
                                    senderName: metadata.agentName || 'Agent',
                                    text: content,
                                    roomId: channel_id, // For SocketIO, room is the central channel_id
                                    serverId: server_id, // Client layer uses serverId
                                    createdAt: new Date(createdMessage.createdAt).getTime(),
                                    source: createdMessage.sourceType,
                                    id: createdMessage.id, // Central message ID
                                    thought: raw_message.thought,
                                    actions: raw_message.actions,
                                    attachments: metadata.attachments,
                                    });
                                }
                                // NO broadcast to internalMessageBus here, this endpoint is for messages ALREADY PROCESSED by an agent
                                // or system messages that don't need further agent processing via the bus.

                                res.status(201).json({ success: true, data: createdMessage });
                                } catch (error) {
                                    std::cerr << '[Messages Router /submit] Error submitting agent message:' << error << std::endl;
                                    res.status(500).json({ success: false, error: 'Failed to submit agent message' });
                                }
                                });

                                // Endpoint to notify that a message is complete (e.g., agent finished responding)
                                (router).post('/complete', async (req: express.Request, res: express.Response) => {
                                    const auto { channel_id, server_id } = req.body;

                                    if (!validateUuid(channel_id) || !validateUuid(server_id)) {
                                        return res.status(400).json({;
                                            success: false,
                                            error: 'Missing or invalid fields: channel_id, server_id',
                                            });
                                        }

                                        try {
                                            if (serverInstance.socketIO) {
                                                serverInstance.socketIO.to(channel_id).emit('messageComplete', {
                                                    channelId: channel_id,
                                                    serverId: server_id,
                                                    });
                                                }

                                                res.status(200).json({ success: true, message: 'Completion event emitted' });
                                                } catch (error) {
                                                    std::cerr << '[Messages Router /notify-complete] Error notifying message complete:' << error << std::endl;
                                                    res.status(500).json({ success: false, error: 'Failed to notify message completion' });
                                                }
                                                });

                                                // Endpoint for INGESTING messages from EXTERNAL platforms (e.g., Discord plugin)
                                                (router).post('/ingest-external', async (req: express.Request, res: express.Response) => {
                                                    const auto messagePayload = req.body<MessageService>; // Partial because ID, created_at will be generated;

                                                    if (
                                                    !messagePayload.channel_id ||;
                                                    !messagePayload.server_id ||;
                                                    !messagePayload.author_id ||;
                                                    !messagePayload.content;
                                                    ) {
                                                        return res.status(400).json({ success: false, error: 'Invalid external message payload' });
                                                    }

                                                    try {
                                                        const auto messageToCreate = {;
                                                            channelId: messagePayload.channel_id,
                                                            authorId: messagePayload.author_id, // This is the original author's ID from the platform (needs mapping to central user ID later)
                                                            content: messagePayload.content,
                                                            rawMessage: messagePayload.raw_message,
                                                            sourceId: messagePayload.source_id, // Original platform message ID
                                                            sourceType: messagePayload.source_type,
                                                            inReplyToRootMessageId: messagePayload.in_reply_to_message_id
                                                            ? validateUuid(messagePayload.in_reply_to_message_id) || std::nullopt;
                                                            : std::nullopt,
                                                            metadata: messagePayload.metadata,
                                                            };
                                                            const auto createdRootMessage = serverInstance.createMessage(messageToCreate);

                                                            // Prepare message for the internal bus (for agents to consume)
                                                            const MessageService messageForBus = {;
                                                                id: createdRootMessage.id!,
                                                                channel_id: createdRootMessage.channelId,
                                                                server_id: messagePayload.server_id, // Pass through the original server_id
                                                                author_id: createdRootMessage.authorId, // This is the central ID used for storage
                                                                author_display_name: messagePayload.author_display_name, // Pass through display name
                                                                content: createdRootMessage.content,
                                                                raw_message: createdRootMessage.rawMessage,
                                                                source_id: createdRootMessage.sourceId,
                                                                source_type: createdRootMessage.sourceType,
                                                                in_reply_to_message_id: createdRootMessage.inReplyToRootMessageId,
                                                                created_at: new Date(createdRootMessage.createdAt).getTime(),
                                                                metadata: createdRootMessage.metadata,
                                                                };

                                                                internalMessageBus.emit('new_message', messageForBus);
                                                                logger.info(
                                                                '[Messages Router /ingest-external] Published to internal message bus:',
                                                                createdRootMessage.id;
                                                                );

                                                                // Also emit to SocketIO for real-time GUI updates if anyone is watching this channel
                                                                if (serverInstance.socketIO) {
                                                                    serverInstance.socketIO.to(messageForBus.channel_id).emit('messageBroadcast', {
                                                                        senderId: messageForBus.author_id,
                                                                        senderName: messageForBus.author_display_name || 'User',
                                                                        text: messageForBus.content,
                                                                        roomId: messageForBus.channel_id,
                                                                        serverId: messageForBus.server_id, // Client layer uses serverId
                                                                        createdAt: messageForBus.created_at,
                                                                        source: messageForBus.source_type,
                                                                        id: messageForBus.id,
                                                                        });
                                                                    }

                                                                    res.status(202).json({
                                                                        success: true,
                                                                        message: 'Message ingested and published to bus',
                                                                        data: { messageId: createdRootMessage.id },
                                                                        });
                                                                        } catch (error) {
                                                                            std::cerr << '[Messages Router /ingest-external] Error ingesting external message:' << error << std::endl;
                                                                            res.status(500).json({ success: false, error: 'Failed to ingest message' });
                                                                        }
                                                                        });

                                                                        return router;

}

} // namespace elizaos
