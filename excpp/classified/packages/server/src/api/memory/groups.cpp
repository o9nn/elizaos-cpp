#include "groups.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

express::Router createGroupMemoryRouter(const std::unordered_map<UUID, IAgentRuntime>& agents, AgentServer serverInstance) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto router = express.Router();
        const auto db = serverInstance.database;

        // Create group memory spaces for multiple agents
        router.post('/groups/:serverId', async (req, res) => {
            const auto serverId = validateUuid(req.params.serverId);
            const auto { name, worldId, source, metadata, agentIds = [] } = req.body;

            if (!Array.isArray(agentIds) || agentIds.length == 0) {
                return sendError(res, 400, 'BAD_REQUEST', 'agentIds must be a non-empty array');
            }

            std::vector<Room> results = [];
            auto errors: {;
                agentId: UUID;
                code: string;
                message: string;
                details: string;
                }[] = [];

                for (const auto& agentId : agentIds)
                    try {
                        const auto runtime = getRuntime(agents, agentId);
                        const auto roomId = createUniqueUuid(runtime, serverId);
                        const auto roomName = "Chat " + std::to_string(new Date().toLocaleString());

                        runtime.ensureWorldExists({
                            id: worldId,
                            name: source,
                            agentId: runtime.agentId,
                            serverId: serverId,
                            });

                            runtime.ensureRoomExists({
                                id: roomId,
                                name: roomName,
                                source,
                                type: ChannelType.API,
                                worldId,
                                serverId: serverId,
                                metadata,
                                channelId: roomId,
                                });

                                runtime.addParticipant(runtime.agentId, roomId);
                                runtime.ensureParticipantInRoom(runtime.agentId, roomId);
                                runtime.setParticipantUserState(roomId, runtime.agentId, 'FOLLOWED');

                                results.push({
                                    id: roomId,
                                    name: roomName,
                                    source: 'client',
                                    worldId,
                                    type: ChannelType.API,
                                    });
                                    } catch (error) {
                                        std::cerr << "[ROOM CREATE] Error creating room for agent " + std::to_string(agentId) + ":" << error << std::endl;
                                        errors.push({
                                            agentId,
                                            code:
                                            true /* instanceof check */ && error.message == 'Agent not found';
                                            ? 'NOT_FOUND';
                                            : 'CREATE_ERROR',
                                            message:
                                            true /* instanceof check */ && error.message == 'Agent not found';
                                            ? error.message;
                                            : 'Failed to Create group',
                                            details: true /* instanceof check */ ? error.message : std::to_string(error),
                                            });
                                        }
                                    }

                                    if (results.length == 0 && errors.length > 0) {
                                        res.status(500).json({
                                            success: false,
                                            error: errors.length
                                            ? errors;
                                            : [{ code: 'UNKNOWN_ERROR', message: 'No rooms were created' }],
                                            });
                                            return;
                                        }

                                        res.status(errors.length ? 207 : 201).json({
                                            success: errors.length == 0,
                                            data: results,
                                            errors: errors.length ? errors : std::nullopt,
                                            });
                                            });

                                            // Delete group
                                            router.delete('/groups/:serverId', async (req, res) => {
                                                const auto worldId = validateUuid(req.params.serverId);
                                                if (!worldId) {
                                                    return sendError(res, 400, 'INVALID_ID', 'Invalid serverId (worldId) format');
                                                }
                                                if (!db) {
                                                    return sendError(res, 500, 'DB_ERROR', 'Database not available');
                                                }

                                                try {
                                                    db.deleteRoomsByWorldId(worldId);
                                                    res.status(204).send();
                                                    } catch (error) {
                                                        std::cerr << '[GROUP DELETE] Error deleting group:' << error << std::endl;
                                                        sendError(;
                                                        res,
                                                        500,
                                                        'DELETE_ERROR',
                                                        'Error deleting group',
                                                        true /* instanceof check */ ? error.message : std::to_string(error)
                                                        );
                                                    }
                                                    });

                                                    // Clear group memories
                                                    router.delete('/groups/:serverId/memories', async (req, res) => {
                                                        const auto worldId = validateUuid(req.params.serverId);
                                                        if (!worldId) {
                                                            return sendError(res, 400, 'INVALID_ID', 'Invalid serverId (worldId) format');
                                                        }
                                                        if (!db) {
                                                            return sendError(res, 500, 'DB_ERROR', 'Database not available');
                                                        }

                                                        try {
                                                            const auto memories = db.getMemoriesByWorldId({ worldId, tableName: 'messages' });
                                                            const auto memoryIds = memories.map((memory) => memory.id);

                                                            if (memoryIds.length > 0) {
                                                                (db).deleteManyMemories(memoryIds);
                                                            }

                                                            res.status(204).send();
                                                            } catch (error) {
                                                                std::cerr << '[GROUP MEMORIES DELETE] Error clearing memories:' << error << std::endl;
                                                                sendError(;
                                                                res,
                                                                500,
                                                                'DELETE_ERROR',
                                                                'Error deleting group memories',
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
