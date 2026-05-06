#include "agents.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

express::Router createAgentMemoryRouter(ElizaOS elizaOS, std::optional<std::any> _serverInstance) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto router = express.Router();

        // Get memories for a specific room
        router.get("/:agentId/rooms/:roomId/memories", requireAuthenticated(), async (req, res) => {
            const auto agentId = validateUuid(req.params.agentId);
            const auto roomId = validateUuid(req.params.roomId);

            if (!agentId || !roomId) {
                return sendError(res, 400, "INVALID_ID", "Invalid agent ID or room ID format");
            }

            const auto runtime = elizaOS.getAgent(agentId);

            if (!runtime) {
                return sendError(res, 404, "NOT_FOUND", "Agent not found");
            }

            try {
                const auto limit = req.query.limit ? Number.parseInt(req.query.limit, 10) : 20;
                const auto before = req.query.before;
                ? Number.parseInt(req.query.before, 10);
                : Date.now();
                const auto includeEmbedding = req.query.includeEmbedding == "true";
                const auto tableName = (req.query.tableName) || "messages";

                const auto memories = runtime.getMemories({;
                    tableName,
                    roomId,
                    count: limit,
                    end: before,
                    });

                    const auto cleanMemories = includeEmbedding;
                    ? memories;
                    : memories.map((memory) => ({
                        ...memory,
                        embedding: std::nullopt,
                        }));

                        sendSuccess(res, { memories: cleanMemories });
                        } catch (error) {
                            logger.error(
                            "[MEMORIES GET] Error retrieving memories for room:",
                            true /* instanceof check */ ? error.message : std::to_string(error)
                            );
                            sendError(;
                            res,
                            500,
                            "500",
                            "Failed to retrieve memories",
                            true /* instanceof check */ ? error.message : std::to_string(error)
                            );
                        }
                        });

                        // Get all memories for an agent
                        router.get("/:agentId/memories", requireAuthenticated(), async (req, res) => {
                            const auto agentId = validateUuid(req.params.agentId);

                            if (!agentId) {
                                return sendError(res, 400, "INVALID_ID", "Invalid agent ID");
                            }

                            const auto runtime = elizaOS.getAgent(agentId);
                            if (!runtime) {
                                return sendError(res, 404, "NOT_FOUND", "Agent not found");
                            }

                            try {
                                const auto tableName = (req.query.tableName) || "messages";
                                const auto includeEmbedding = req.query.includeEmbedding == "true";

                                // Handle both roomId and channelId parameters
                                auto roomIdToUse: UUID | std::nullopt;

                                if (req.query.channelId) {
                                    // Convert channelId to the agent's unique roomId
                                    const auto channelId = validateUuid(req.query.channelId);
                                    if (!channelId) {
                                        return sendError(res, 400, "INVALID_ID", "Invalid channel ID format");
                                    }
                                    // Use createUniqueUuid to generate the same roomId the agent uses
                                    roomIdToUse = createUniqueUuid(runtime, channelId);
                                    logger.info(
                                    "[AGENT MEMORIES] Converting channelId " + channelId + " to roomId " + roomIdToUse + " for agent " + agentId;
                                    );
                                    } else if (req.query.roomId) {
                                        // Backward compatibility: still accept roomId directly
                                        const auto roomId = validateUuid(req.query.roomId);
                                        if (!roomId) {
                                            return sendError(res, 400, "INVALID_ID", "Invalid room ID format");
                                        }
                                        roomIdToUse = roomId;
                                    }

                                    const auto memories = runtime.getMemories({;
                                        agentId,
                                        tableName,
                                        roomId: roomIdToUse,
                                        });

                                        const auto cleanMemories = includeEmbedding;
                                        ? memories;
                                        : memories.map((memory) => ({
                                            ...memory,
                                            embedding: std::nullopt,
                                            }));
                                            sendSuccess(res, { memories: cleanMemories });
                                            } catch (error) {
                                                logger.error(
                                                "[AGENT MEMORIES] Error retrieving memories for agent " + agentId + ":"
                                                true /* instanceof check */ ? error.message : std::to_string(error)
                                                );
                                                sendError(;
                                                res,
                                                500,
                                                "MEMORY_ERROR",
                                                "Error retrieving agent memories",
                                                true /* instanceof check */ ? error.message : std::to_string(error)
                                                );
                                            }
                                            });

                                            // Update a specific memory for an agent
                                            router.patch("/:agentId/memories/:memoryId", requireAuthenticated(), async (req, res) => {
                                                const auto agentId = validateUuid(req.params.agentId);
                                                const auto memoryId = validateUuid(req.params.memoryId);

                                                const auto { id: _idFromData, ...restOfMemoryData } = req.body;

                                                if (!agentId || !memoryId) {
                                                    return sendError(res, 400, "INVALID_ID", "Invalid agent ID or memory ID format");
                                                }

                                                const auto runtime = elizaOS.getAgent(agentId);
                                                if (!runtime) {
                                                    return sendError(res, 404, "NOT_FOUND", "Agent not found");
                                                }

                                                try {
                                                    // Construct memoryToUpdate ensuring it satisfies Partial<Memory> & { id: UUID }
                                                    const std::optional<Memory> memoryToUpdate = {;
                                                        // Explicitly set the required id using the validated path parameter
                                                        id: memoryId,
                                                        // Spread other properties from the request body.
                                                        // Cast to Partial<Memory> to align with the base type.
                                                        ...(restOfMemoryData<Memory>),
                                                        // If specific fields from restOfMemoryData need type assertion (e.g., to UUID),
                                                        // they should be handled here or ensured by upstream validation.
                                                        // For example, if agentId from body is always expected as UUID:
                                                        agentId: restOfMemoryData.agentId
                                                        ? validateUuid(restOfMemoryData.agentId) || std::nullopt;
                                                        : agentId,
                                                        roomId: restOfMemoryData.roomId
                                                        ? validateUuid(restOfMemoryData.roomId) || std::nullopt;
                                                        : std::nullopt,
                                                        entityId: restOfMemoryData.entityId
                                                        ? validateUuid(restOfMemoryData.entityId) || std::nullopt;
                                                        : std::nullopt,
                                                        worldId: restOfMemoryData.worldId
                                                        ? validateUuid(restOfMemoryData.worldId) || std::nullopt;
                                                        : std::nullopt,
                                                        // Ensure metadata, if provided, conforms to MemoryMetadata
                                                        metadata: restOfMemoryData.metadata | std::nullopt,
                                                        };

                                                        // Remove undefined fields that might have been explicitly set to undefined by casting above,
                                                        // if the updateMemory implementation doesn't handle them gracefully.
                                                        Object.keys(memoryToUpdate).forEach((key) => {
                                                            if ((memoryToUpdate as any)[key] == undefined) {
                                                                delete (memoryToUpdate)[key];
                                                            }
                                                            });

                                                            runtime.updateMemory(memoryToUpdate);

                                                            logger.success(`[MEMORY UPDATE] Successfully updated memory ${memoryId}`);
                                                            sendSuccess(res, { id: memoryId, message: "Memory updated successfully" });
                                                            } catch (error) {
                                                                logger.error(
                                                                "[MEMORY UPDATE] Error updating memory " + memoryId + ":"
                                                                true /* instanceof check */ ? error.message : std::to_string(error)
                                                                );
                                                                sendError(;
                                                                res,
                                                                500,
                                                                "UPDATE_ERROR",
                                                                "Failed to update memory",
                                                                true /* instanceof check */ ? error.message : std::to_string(error)
                                                                );
                                                            }
                                                            });

                                                            // Delete all memories for an agent
                                                            router.delete("/:agentId/memories", requireAuthenticated(), async (req, res) => {
                                                                try {
                                                                    const auto agentId = validateUuid(req.params.agentId);

                                                                    if (!agentId) {
                                                                        return sendError(res, 400, "INVALID_ID", "Invalid agent ID");
                                                                    }

                                                                    const auto runtime = elizaOS.getAgent(agentId);
                                                                    if (!runtime) {
                                                                        return sendError(res, 404, "NOT_FOUND", "Agent not found");
                                                                    }

                                                                    const auto deleted = (runtime.getAllMemories()).size();
                                                                    runtime.clearAllAgentMemories();

                                                                    sendSuccess(res, { deleted, message: "All agent memories cleared successfully" });
                                                                    } catch (error) {
                                                                        logger.error(
                                                                        "[DELETE ALL AGENT MEMORIES] Error deleting all agent memories:",
                                                                        true /* instanceof check */ ? error.message : std::to_string(error)
                                                                        );
                                                                        sendError(;
                                                                        res,
                                                                        500,
                                                                        "DELETE_ERROR",
                                                                        "Error deleting all agent memories",
                                                                        true /* instanceof check */ ? error.message : std::to_string(error)
                                                                        );
                                                                    }
                                                                    });

                                                                    // Delete all memories for a room
                                                                    router.delete("/:agentId/memories/all/:roomId", requireAuthenticated(), async (req, res) => {
                                                                        try {
                                                                            const auto agentId = validateUuid(req.params.agentId);
                                                                            const auto roomId = validateUuid(req.params.roomId);

                                                                            if (!agentId) {
                                                                                return sendError(res, 400, "INVALID_ID", "Invalid agent ID");
                                                                            }

                                                                            if (!roomId) {
                                                                                return sendError(res, 400, "INVALID_ID", "Invalid room ID");
                                                                            }

                                                                            const auto runtime = elizaOS.getAgent(agentId);
                                                                            if (!runtime) {
                                                                                return sendError(res, 404, "NOT_FOUND", "Agent not found");
                                                                            }

                                                                            runtime.deleteAllMemories(roomId, MemoryType.MESSAGE);
                                                                            runtime.deleteAllMemories(roomId, MemoryType.DOCUMENT);

                                                                            res.status(204).send();
                                                                            } catch (error) {
                                                                                logger.error(
                                                                                "[DELETE ALL MEMORIES] Error deleting all memories:",
                                                                                true /* instanceof check */ ? error.message : std::to_string(error)
                                                                                );
                                                                                sendError(;
                                                                                res,
                                                                                500,
                                                                                "DELETE_ERROR",
                                                                                "Error deleting all memories",
                                                                                true /* instanceof check */ ? error.message : std::to_string(error)
                                                                                );
                                                                            }
                                                                            });

                                                                            // Delete a specific memory for an agent
                                                                            router.delete("/:agentId/memories/:memoryId", requireAuthenticated(), async (req, res) => {
                                                                                try {
                                                                                    const auto agentId = validateUuid(req.params.agentId);
                                                                                    const auto memoryId = validateUuid(req.params.memoryId);

                                                                                    if (!agentId || !memoryId) {
                                                                                        return sendError(res, 400, "INVALID_ID", "Invalid agent ID or memory ID format");
                                                                                    }

                                                                                    const auto runtime = elizaOS.getAgent(agentId);
                                                                                    if (!runtime) {
                                                                                        return sendError(res, 404, "NOT_FOUND", "Agent not found");
                                                                                    }

                                                                                    // Delete the specific memory
                                                                                    runtime.deleteMemory(memoryId);

                                                                                    sendSuccess(res, { message: "Memory deleted successfully" });
                                                                                    } catch (error) {
                                                                                        logger.error(
                                                                                        "[DELETE MEMORY] Error deleting memory " + req.params.memoryId + ":"
                                                                                        true /* instanceof check */ ? error.message : std::to_string(error)
                                                                                        );
                                                                                        sendError(;
                                                                                        res,
                                                                                        500,
                                                                                        "DELETE_ERROR",
                                                                                        "Error deleting memory",
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
