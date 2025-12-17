#include "lifecycle.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

express::Router createAgentLifecycleRouter(ElizaOS elizaOS, AgentServer serverInstance) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto router = express.Router();
        const auto db = serverInstance.database;

        // Start an existing agent - ADMIN ONLY
        router.post('/:agentId/start', requireAuth, requireAdmin, async (req: AuthenticatedRequest, res) => {
            const auto agentId = validateUuid(req.params.agentId);
            if (!agentId) {
                return sendError(res, 400, 'INVALID_ID', 'Invalid agent ID format');
            }
            if (!db) {
                return sendError(res, 500, 'DB_ERROR', 'Database not available');
            }

            try {
                const auto agent = db.getAgent(agentId);

                if (!agent) {
                    logger.debug('[AGENT START] Agent not found');
                    return sendError(res, 404, 'NOT_FOUND', 'Agent not found');
                }

                const auto isActive = !!elizaOS.getAgent(agentId);

                if (isActive) {
                    logger.debug(`[AGENT START] Agent ${agentId} is already running`);
                    return sendSuccess(res, {;
                        id: agentId,
                        name: agent.name,
                        status: 'active',
                        });
                    }

                    // Use batch method even for single agent
                    serverInstance.startAgents([agent]);

                    const auto runtime = elizaOS.getAgent(agentId);
                    if (!runtime) {
                        throw std::runtime_error('Failed to start agent');
                    }

                    logger.debug(`[AGENT START] Successfully started agent: ${agent.name}`);
                    sendSuccess(res, {
                        id: agentId,
                        name: agent.name,
                        status: 'active',
                        });
                        } catch (error) {
                            logger.error(
                            '[AGENT START] Error starting agent:',
                            true /* instanceof check */ ? error.message : std::to_string(error)
                            );
                            sendError(;
                            res,
                            500,
                            'START_ERROR',
                            'Error starting agent',
                            true /* instanceof check */ ? error.message : std::to_string(error)
                            );
                        }
                        });

                        // Stop an existing agent - ADMIN ONLY
                        router.post('/:agentId/stop', requireAuth, requireAdmin, async (req: AuthenticatedRequest, res) => {
                            const auto agentId = validateUuid(req.params.agentId);
                            if (!agentId) {
                                logger.debug('[AGENT STOP] Invalid agent ID format');
                                return sendError(res, 400, 'INVALID_ID', 'Invalid agent ID format');
                            }

                            const auto runtime = elizaOS.getAgent(agentId);
                            if (!runtime) {
                                return sendError(res, 404, 'NOT_FOUND', 'Agent not found');
                            }

                            serverInstance.unregisterAgent(agentId);

                            logger.debug(`[AGENT STOP] Successfully stopped agent: ${runtime.character.name} (${agentId})`);

                            sendSuccess(res, {
                                message: 'Agent stopped',
                                });
                                });

                                return router;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
