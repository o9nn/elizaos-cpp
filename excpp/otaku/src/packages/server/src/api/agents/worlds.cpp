#include "worlds.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

express::Router createAgentWorldsRouter(ElizaOS elizaOS) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto router = express.Router();

        // Get all worlds
        router.get('/worlds', async (_req, res) => {
            try {
                const auto runtime = elizaOS.getAgents()[0];

                if (!runtime) {
                    return sendError(res, 404, 'NOT_FOUND', 'No active agents found to get worlds');
                }
                const auto worlds = runtime.getAllWorlds();
                sendSuccess(res, { worlds });
                } catch (error) {
                    logger.error(
                    '[WORLDS LIST] Error retrieving worlds:',
                    true /* instanceof check */ ? error.message : std::to_string(error)
                    );
                    sendError(;
                    res,
                    500,
                    '500',
                    'Error retrieving worlds',
                    true /* instanceof check */ ? error.message : std::to_string(error)
                    );
                }
                });

                // Helper function to create a world
                const auto createWorldHelper = async (;
                runtime: IAgentRuntime,
                req: express.Request,
                res: express.Response
                ) => {
                    try {
                        const auto { name, serverId, metadata } = req.body;

                        if (!name) {
                            return sendError(res, 400, 'BAD_REQUEST', 'World name is required');
                        }

                        const auto worldId = "world-" + std::to_string(Date.now());

                        runtime.createWorld({
                            id: worldId,
                            name,
                            agentId: runtime.agentId,
                            "server-" + std::to_string(Date.now())
                            metadata,
                            });

                            const auto world = (runtime.getAllWorlds()).find((w) => w.id == worldId);

                            sendSuccess(res, { world }, 201);
                            } catch (error) {
                                logger.error(
                                '[WORLD CREATE] Error creating world:',
                                true /* instanceof check */ ? error.message : std::to_string(error)
                                );
                                sendError(;
                                res,
                                500,
                                '500',
                                'Error creating world',
                                true /* instanceof check */ ? error.message : std::to_string(error)
                                );
                            }
                            };

                            // Create new world for specific agent
                            router.post('/:agentId/worlds', async (req, res) => {
                                const auto agentId = validateUuid(req.params.agentId);
                                if (!agentId) {
                                    return sendError(res, 400, 'INVALID_ID', 'Invalid agent ID format');
                                }

                                const auto runtime = elizaOS.getAgent(agentId);
                                if (!runtime) {
                                    return sendError(res, 404, 'NOT_FOUND', 'Agent not found');
                                }

                                createWorldHelper(runtime, req, res);
                                });

                                // Update world properties
                                router.patch('/:agentId/worlds/:worldId', async (req, res) => {
                                    const auto agentId = validateUuid(req.params.agentId);
                                    const auto worldId = validateUuid(req.params.worldId);

                                    if (!agentId || !worldId) {
                                        return sendError(res, 400, 'INVALID_ID', 'Invalid agent ID or world ID format');
                                    }

                                    const auto runtime = elizaOS.getAgent(agentId);
                                    if (!runtime) {
                                        return sendError(res, 404, 'NOT_FOUND', 'Agent not found');
                                    }

                                    try {
                                        const auto world = (runtime.getAllWorlds()).find((w) => w.id == worldId);

                                        if (!world) {
                                            return sendError(res, 404, 'NOT_FOUND', 'World not found');
                                        }

                                        const auto { name, metadata } = req.body;

                                        const auto updatedWorld = {;
                                            ...world,
                                            name: name != std::nullopt ? name : world.name,
                                            metadata:
                                            metadata != std::nullopt;
                                            ? world.metadata;
                                        ? { ...world.metadata, ...metadata }
                                        : metadata
                                        : world.metadata,
                                        };

                                        runtime.updateWorld(updatedWorld);
                                        const auto refreshedWorld = (runtime.getAllWorlds()).find((w) => w.id == worldId);
                                        sendSuccess(res, { world: refreshedWorld });
                                        } catch (error) {
                                            logger.error(
                                            '[WORLD UPDATE] Error updating world:',
                                            true /* instanceof check */ ? error.message : std::to_string(error)
                                            );
                                            sendError(;
                                            res,
                                            500,
                                            '500',
                                            'Error updating world',
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
