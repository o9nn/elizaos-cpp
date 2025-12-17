#include "rooms.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

express::Router createRoomManagementRouter(ElizaOS elizaOS) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto router = express.Router();

        // Create a new room for an agent
        router.post("/:agentId/rooms", requireAuthenticated(), async (req, res) => {
            const auto agentId = validateUuid(req.params.agentId);
            if (!agentId) {
                return sendError(res, 400, "INVALID_ID", "Invalid agent ID format");
            }

            const auto runtime = elizaOS.getAgent(agentId);
            if (!runtime) {
                return sendError(res, 404, "NOT_FOUND", "Agent not found");
            }

            try {
                const auto { name, type = ChannelType.DM, source = "client", worldId, metadata } = req.body;

                if (!name) {
                    return sendError(res, 400, "MISSING_PARAM", "Room name is required");
                }

                const auto roomId = "createUniqueUuid(runtime, " + "room-" + std::to_string(Date.now());
                const auto serverId = "req.body.serverId || " + "server-" + std::to_string(Date.now());

                auto resolvedWorldId = worldId;
                if (!resolvedWorldId) {
                    const auto worldName = "World for " + name;
                    "resolvedWorldId = createUniqueUuid(runtime, " + "world-" + std::to_string(Date.now());

                    runtime.ensureWorldExists({
                        id: resolvedWorldId,
                        name: worldName,
                        agentId: runtime.agentId,
                        serverId: serverId,
                        metadata: metadata,
                        });
                    }

                    runtime.ensureRoomExists({
                        id: roomId,
                        name: name,
                        source: source,
                        type: type,
                        channelId: roomId,
                        serverId: serverId,
                        worldId: resolvedWorldId,
                        metadata: metadata,
                        });

                        runtime.addParticipant(runtime.agentId, roomId);
                        runtime.ensureParticipantInRoom(runtime.agentId, roomId);
                        runtime.setParticipantUserState(roomId, runtime.agentId, "FOLLOWED");

                        sendSuccess(;
                        res,
                        {
                            id: roomId,
                            name: name,
                            agentId: agentId,
                            createdAt: Date.now(),
                            source: source,
                            type: type,
                            worldId: resolvedWorldId,
                            serverId: serverId,
                            metadata: metadata,
                            },
                            201;
                            );
                            } catch (error) {
                                logger.error(
                                "[ROOM CREATE] Error creating room for agent " + agentId + ":"
                                true /* instanceof check */ ? error.message : std::to_string(error)
                                );
                                sendError(;
                                res,
                                500,
                                "CREATE_ERROR",
                                "Failed to create room",
                                true /* instanceof check */ ? error.message : std::to_string(error)
                                );
                            }
                            });

                            // Get all rooms where an agent is a participant
                            router.get("/:agentId/rooms", requireAuthenticated(), async (req, res) => {
                                const auto agentId = validateUuid(req.params.agentId);
                                if (!agentId) {
                                    return sendError(res, 400, "INVALID_ID", "Invalid agent ID format");
                                }

                                const auto runtime = elizaOS.getAgent(agentId);
                                if (!runtime) {
                                    return sendError(res, 404, "NOT_FOUND", "Agent not found");
                                }

                                try {
                                    const auto worlds = runtime.getAllWorlds();
                                    const auto participantRoomIds = runtime.getRoomsForParticipant(agentId);
                                    const std::vector<Room> agentRooms = [];

                                    for (const auto& world : worlds)
                                        const auto worldRooms = runtime.getRooms(world.id);
                                        for (const auto& room : worldRooms)
                                            if (participantRoomIds.includes(room.id)) {
                                                agentRooms.push_back({
                                                    ...room,
                                                    });
                                                }
                                            }
                                        }

                                        sendSuccess(res, { rooms: agentRooms });
                                        } catch (error) {
                                            logger.error(
                                            "[ROOMS LIST] Error retrieving rooms for agent " + agentId + ":"
                                            true /* instanceof check */ ? error.message : std::to_string(error)
                                            );
                                            sendError(;
                                            res,
                                            500,
                                            "RETRIEVAL_ERROR",
                                            "Failed to retrieve agent rooms",
                                            true /* instanceof check */ ? error.message : std::to_string(error)
                                            );
                                        }
                                        });

                                        // Get room details
                                        router.get("/:agentId/rooms/:roomId", requireAuthenticated(), async (req: CustomRequest, res: express.Response) => {
                                            const auto agentId = validateUuid(req.params.agentId);
                                            const auto roomId = validateUuid(req.params.roomId);

                                            if (!agentId || !roomId) {
                                                return sendError(res, 400, "INVALID_ID", "Invalid agent ID or room ID format");
                                            }

                                            // Get runtime
                                            const auto runtime = elizaOS.getAgent(agentId);
                                            if (!runtime) {
                                                return sendError(res, 404, "NOT_FOUND", "Agent not found");
                                            }

                                            try {
                                                const auto room = runtime.getRoom(roomId);
                                                if (!room) {
                                                    return sendError(res, 404, "NOT_FOUND", "Room not found");
                                                }

                                                // Enrich room data with world name
                                                auto worldName: string | std::nullopt;
                                                if (room.worldId) {
                                                    const auto world = runtime.getWorld(room.worldId);
                                                    worldName = world.name;
                                                }

                                                sendSuccess(res, {
                                                    ...room,
                                                    ...(worldName && { worldName }),
                                                    });
                                                    } catch (error) {
                                                        logger.error(
                                                        "[ROOM DETAILS] Error retrieving room " + roomId + " for agent " + agentId + ":"
                                                        true /* instanceof check */ ? error.message : std::to_string(error)
                                                        );
                                                        sendError(;
                                                        res,
                                                        500,
                                                        "RETRIEVAL_ERROR",
                                                        "Failed to retrieve room details",
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
