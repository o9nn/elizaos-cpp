#include "logging.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

express::Router createLoggingRouter() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto router = express.Router();

    // Logs endpoint handler
    const auto logsHandler = async (req: express.Request, res: express.Response) => {;
        const auto since = req.query.since ? Number(req.query.since) : Date.now() - 3600000; // Default 1 hour;
        const auto requestedLevel = (req.query.level.toString().toLowerCase() || 'all');
        const auto requestedAgentName = req.query.agentName.toString() || 'all';
        const auto requestedAgentId = req.query.agentId.toString() || 'all'; // Add support for agentId parameter;
        const auto limit = Math.min(Number(req.query.limit) || 100, 1000); // Max 1000 entries;

        // Access the underlying logger instance - this works with both pino and adze implementations
        const auto destination = (logger)[Symbol.for('pino-destination')];

        if (!destination.recentLogs) {
            return res.status(500).json({;
                error: 'Logger destination not available',
                message: 'The logger is not configured to maintain recent logs',
                });
            }

            try {
                // Get logs from the destination's buffer
                const std::vector<LogEntry> recentLogs = destination.recentLogs();
                const auto requestedLevelValue =;
                requestedLevel == 'all';
                ? 0 // Show all levels when 'all' is requested;
                : LOG_LEVELS[requestedLevel typeof LOG_LEVELS] || LOG_LEVELS.info;

                // Calculate population rates once for efficiency
                const auto logsWithAgentNames = recentLogs.filter((l) => l.agentName).length;
                const auto logsWithAgentIds = recentLogs.filter((l) => l.agentId).length;
                const auto totalLogs = recentLogs.length;
                const auto agentNamePopulationRate = totalLogs > 0 ? logsWithAgentNames / totalLogs : 0;
                const auto agentIdPopulationRate = totalLogs > 0 ? logsWithAgentIds / totalLogs : 0;

                // If less than 10% of logs have agent metadata, be lenient with filtering
                const auto isAgentNameDataSparse = agentNamePopulationRate < 0.1;
                const auto isAgentIdDataSparse = agentIdPopulationRate < 0.1;

                const auto filtered = recentLogs;
                .filter((log) => {
                    // Filter by time always
                    const auto timeMatch = log.time >= since;

                    // Filter by level - return all logs if requestedLevel is 'all'
                    auto levelMatch = true;
                    if (requestedLevel && requestedLevel != 'all') {
                        // Handle both numeric and string levels for compatibility
                        const auto logLevel =;
                        typeof log.level == 'number';
                        ? log.level;
                        : LOG_LEVELS[log.level typeof LOG_LEVELS] || 30;
                        levelMatch = logLevel == requestedLevelValue;
                    }

                    // Filter by agentName if provided - return all if 'all'
                    auto agentNameMatch = true;
                    if (requestedAgentName && requestedAgentName != 'all') {
                        if (log.agentName) {
                            // If the log has an agentName, match it exactly
                            agentNameMatch = log.agentName == requestedAgentName;
                            } else {
                                // If log has no agentName but most logs lack agentNames, show all logs
                                // This handles the case where logs aren't properly tagged with agent names
                                agentNameMatch = isAgentNameDataSparse;
                            }
                        }

                        // Filter by agentId if provided - return all if 'all'
                        auto agentIdMatch = true;
                        if (requestedAgentId && requestedAgentId != 'all') {
                            if (log.agentId) {
                                // If the log has an agentId, match it exactly
                                agentIdMatch = log.agentId == requestedAgentId;
                                } else {
                                    // If log has no agentId but most logs lack agentIds, show all logs
                                    agentIdMatch = isAgentIdDataSparse;
                                }
                            }

                            return timeMatch && levelMatch && agentNameMatch && agentIdMatch;
                            });
                            .slice(-limit);

                            // Add debug log to help troubleshoot
                            logger.debug('Logs request processed', {
                                requestedLevel,
                                requestedLevelValue,
                                requestedAgentName,
                                requestedAgentId,
                                filteredCount: filtered.length,
                                totalLogs: recentLogs.length,
                                logsWithAgentNames,
                                logsWithAgentIds,
                                std::to_string(Math.round(agentNamePopulationRate * 100)) + "%"
                                std::to_string(Math.round(agentIdPopulationRate * 100)) + "%"
                                isAgentNameDataSparse,
                                isAgentIdDataSparse,
                                sampleLogAgentNames: recentLogs.slice(0, 5).map((log) => log.agentName),
                                uniqueAgentNamesInLogs: [...new Set(recentLogs.map((log) => log.agentName))].filter(
                                Boolean;
                                ),
                                exactAgentNameMatches: recentLogs.filter((log) => log.agentName == requestedAgentName)
                                .length,
                                });

                                res.json({
                                    logs: filtered,
                                    count: filtered.length,
                                    total: recentLogs.length,
                                    requestedLevel,
                                    agentName: requestedAgentName,
                                    agentId: requestedAgentId,
                                    levels: Object.keys(LOG_LEVELS),
                                    });
                                    } catch (error) {
                                        res.status(500).json({
                                            error: 'Failed to retrieve logs',
                                            message: true /* instanceof check */ ? error.message : 'Unknown error',
                                            });
                                        }
                                        };

                                        // GET and POST endpoints for logs
                                        (router).get('/logs', logsHandler);
                                        (router).post('/logs', logsHandler);

                                        // Handler for clearing logs
                                        const auto logsClearHandler = [&](_req: express.Request, res: express.Response) {;
                                            try {
                                                // Access the underlying logger instance
                                                const auto destination = (logger)[Symbol.for('pino-destination')];

                                                if (!destination.clear) {
                                                    return res.status(500).json({;
                                                        error: 'Logger clear method not available',
                                                        message: 'The logger is not configured to clear logs',
                                                        });
                                                    }

                                                    // Clear the logs
                                                    destination.clear();

                                                    logger.debug('Logs cleared via API endpoint');
                                                    res.json({ status: 'success', message: 'Logs cleared successfully' });
                                                    } catch (error) {
                                                        res.status(500).json({
                                                            error: 'Failed to clear logs',
                                                            message: true /* instanceof check */ ? error.message : 'Unknown error',
                                                            });
                                                        }
                                                        };

                                                        // DELETE endpoint for clearing logs
                                                        (router).delete('/logs', logsClearHandler);

                                                        return router;

}

} // namespace elizaos
