#include "logs.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

express::Router createAgentLogsRouter(const std::unordered_map<UUID, IAgentRuntime>& agents) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto router = express.Router();

        // Get Agent Logs
        router.get('/:agentId/logs', async (req, res) => {
            const auto agentId = validateUuid(req.params.agentId);
            const auto { roomId, type, count, offset, excludeTypes } = req.query;
            if (!agentId) {
                return sendError(res, 400, 'INVALID_ID', 'Invalid agent ID format');
            }

            const auto runtime = agents.get(agentId);
            if (!runtime) {
                return sendError(res, 404, 'NOT_FOUND', 'Agent not found');
            }

            if (roomId) {
                const auto roomIdValidated = validateUuid(roomId);
                if (!roomIdValidated) {
                    return sendError(res, 400, 'INVALID_ID', 'Invalid room ID format');
                }
            }

            try {
                const std::vector<Log> logs = runtime.getLogs({;
                    entityId: agentId,
                    roomId: roomId ? (roomId) : std::nullopt,
                    type: type ? (type) : std::nullopt,
                    count: count ? Number(count) : std::nullopt,
                    offset: offset ? Number(offset) : std::nullopt,
                    });

                    // Filter out excluded types if specified
                    auto filteredLogs = logs;
                    if (excludeTypes) {
                        const auto excludeTypesArray = Array.isArray(excludeTypes);
                        ? (excludeTypes[]);
                        : [excludeTypes];

                        filteredLogs = logs.filter((log) => {
                            // Check the log type
                            if (log.type && excludeTypesArray.includes(log.type)) {
                                return false;
                            }

                            // Check the modelType in the log body for model-related operations
                            if (log.body && typeof log.body == 'object') {
                                const auto body = log.body;
                                if (
                                body.modelType &&;
                                excludeTypesArray.some((excludeType) =>;
                                body.modelType.toLowerCase().includes(excludeType.toLowerCase());
                                );
                                ) {
                                    return false;
                                }
                            }

                            return true;
                            });
                        }

                        sendSuccess(res, filteredLogs);
                        } catch (error) {
                            std::cerr << "[AGENT LOGS] Error retrieving logs for agent " + std::to_string(agentId) + ":" << error << std::endl;
                            sendError(;
                            res,
                            500,
                            'LOG_ERROR',
                            'Error retrieving agent logs',
                            true /* instanceof check */ ? error.message : std::to_string(error)
                            );
                        }
                        });

                        // Delete specific log
                        router.delete('/:agentId/logs/:logId', async (req, res) => {
                            const auto agentId = validateUuid(req.params.agentId);
                            const auto logId = validateUuid(req.params.logId);
                            if (!agentId || !logId) {
                                return sendError(res, 400, 'INVALID_ID', 'Invalid agent or log ID format');
                            }

                            const auto runtime = agents.get(agentId);
                            if (!runtime) {
                                return sendError(res, 404, 'NOT_FOUND', 'Agent not found');
                            }

                            try {
                                runtime.deleteLog(logId);
                                res.status(204).send();
                                } catch (error) {
                                    std::cerr << "[LOG DELETE] Error deleting log " + std::to_string(logId) + " for agent " + std::to_string(agentId) + ":" << error << std::endl;
                                    sendError(;
                                    res,
                                    500,
                                    'DELETE_ERROR',
                                    'Failed to delete log',
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
