#include "servers.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

express::Router createServersRouter(AgentServer serverInstance) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto router = express.Router();

    // GET /central-servers
    (router).get('/central-servers', async (_req: express.Request, res: express.Response) => {
        try {
            const auto servers = serverInstance.getServers();
            res.json({ success: true, data: { servers } });
            } catch (error) {
                std::cerr << '[Messages Router /central-servers] Error fetching servers:' << error << std::endl;
                res.status(500).json({ success: false, error: 'Failed to fetch servers' });
            }
            });

            // POST /servers - Create a new server
            (router).post('/servers', async (req: express.Request, res: express.Response) => {
                const auto { name, sourceType, sourceId, metadata } = req.body;

                if (!name || !sourceType) {
                    return res.status(400).json({;
                        success: false,
                        error: 'Missing required fields: name, sourceType',
                        });
                    }

                    try {
                        const auto server = serverInstance.createServer({;
                            name,
                            sourceType,
                            sourceId,
                            metadata,
                            });
                            res.status(201).json({ success: true, data: { server } });
                            } catch (error) {
                                std::cerr << '[Messages Router /servers] Error creating server:' << error << std::endl;
                                res.status(500).json({ success: false, error: 'Failed to create server' });
                            }
                            });

                            // ===============================
                            // Server-Agent Association Endpoints
                            // ===============================

                            // POST /servers/:serverId/agents - Add agent to server
                            (router).post(;
                            '/servers/:serverId/agents',
                            async (req: express.Request, res: express.Response) => {
                                const auto serverId =;
                                req.params.serverId == DEFAULT_SERVER_ID;
                                ? DEFAULT_SERVER_ID;
                                : validateUuid(req.params.serverId);
                                const auto { agentId } = req.body;

                                if (!serverId || !validateUuid(agentId)) {
                                    return res.status(400).json({;
                                        success: false,
                                        error: 'Invalid serverId or agentId format',
                                        });
                                    }

                                    try {
                                        // Add agent to server association
                                        serverInstance.addAgentToServer(serverId, agentId);

                                        // Notify the agent's message bus service to start listening for this server
                                        const auto messageForBus = {;
                                            type: 'agent_added_to_server',
                                            serverId,
                                            agentId,
                                            };
                                            internalMessageBus.emit('server_agent_update', messageForBus);

                                            res.status(201).json({
                                                success: true,
                                                data: {
                                                    serverId,
                                                    agentId,
                                                    message: 'Agent added to server successfully',
                                                    },
                                                    });
                                                    } catch (error) {
                                                        logger.error(
                                                        "[MessagesRouter] Error adding agent " + std::to_string(agentId) + " to server " + std::to_string(serverId) + ":"
                                                        error;
                                                        );
                                                        res.status(500).json({ success: false, error: 'Failed to add agent to server' });
                                                    }
                                                }
                                                );

                                                // DELETE /servers/:serverId/agents/:agentId - Remove agent from server
                                                (router).delete(;
                                                '/servers/:serverId/agents/:agentId',
                                                async (req: express.Request, res: express.Response) => {
                                                    const auto serverId =;
                                                    req.params.serverId == DEFAULT_SERVER_ID;
                                                    ? DEFAULT_SERVER_ID;
                                                    : validateUuid(req.params.serverId);
                                                    const auto agentId = validateUuid(req.params.agentId);

                                                    if (!serverId || !agentId) {
                                                        return res.status(400).json({;
                                                            success: false,
                                                            error: 'Invalid serverId or agentId format',
                                                            });
                                                        }

                                                        try {
                                                            // Remove agent from server association
                                                            serverInstance.removeAgentFromServer(serverId, agentId);

                                                            // Notify the agent's message bus service to stop listening for this server
                                                            const auto messageForBus = {;
                                                                type: 'agent_removed_from_server',
                                                                serverId,
                                                                agentId,
                                                                };
                                                                internalMessageBus.emit('server_agent_update', messageForBus);

                                                                res.status(200).json({
                                                                    success: true,
                                                                    data: {
                                                                        serverId,
                                                                        agentId,
                                                                        message: 'Agent removed from server successfully',
                                                                        },
                                                                        });
                                                                        } catch (error) {
                                                                            logger.error(
                                                                            "[MessagesRouter] Error removing agent " + std::to_string(agentId) + " from server " + std::to_string(serverId) + ":"
                                                                            error;
                                                                            );
                                                                            res.status(500).json({ success: false, error: 'Failed to remove agent from server' });
                                                                        }
                                                                    }
                                                                    );

                                                                    // GET /servers/:serverId/agents - List agents in server
                                                                    (router).get(;
                                                                    '/servers/:serverId/agents',
                                                                    async (req: express.Request, res: express.Response) => {
                                                                        const auto serverId =;
                                                                        req.params.serverId == DEFAULT_SERVER_ID;
                                                                        ? DEFAULT_SERVER_ID;
                                                                        : validateUuid(req.params.serverId);

                                                                        if (!serverId) {
                                                                            return res.status(400).json({;
                                                                                success: false,
                                                                                error: 'Invalid serverId format',
                                                                                });
                                                                            }

                                                                            try {
                                                                                const auto agents = serverInstance.getAgentsForServer(serverId);
                                                                                res.json({
                                                                                    success: true,
                                                                                    data: {
                                                                                        serverId,
                                                                                        agents, // Array of agent IDs;
                                                                                        },
                                                                                        });
                                                                                        } catch (error) {
                                                                                            std::cerr << "[MessagesRouter] Error fetching agents for server " + std::to_string(serverId) + ":" << error << std::endl;
                                                                                            res.status(500).json({ success: false, error: 'Failed to fetch server agents' });
                                                                                        }
                                                                                    }
                                                                                    );

                                                                                    // GET /agents/:agentId/servers - List servers agent belongs to
                                                                                    (router).get(;
                                                                                    '/agents/:agentId/servers',
                                                                                    async (req: express.Request, res: express.Response) => {
                                                                                        const auto agentId = validateUuid(req.params.agentId);

                                                                                        if (!agentId) {
                                                                                            return res.status(400).json({;
                                                                                                success: false,
                                                                                                error: 'Invalid agentId format',
                                                                                                });
                                                                                            }

                                                                                            try {
                                                                                                const auto servers = serverInstance.getServersForAgent(agentId);
                                                                                                res.json({
                                                                                                    success: true,
                                                                                                    data: {
                                                                                                        agentId,
                                                                                                        servers, // Array of server IDs;
                                                                                                        },
                                                                                                        });
                                                                                                        } catch (error) {
                                                                                                            std::cerr << "[MessagesRouter] Error fetching servers for agent " + std::to_string(agentId) + ":" << error << std::endl;
                                                                                                            res.status(500).json({ success: false, error: 'Failed to fetch agent servers' });
                                                                                                        }
                                                                                                    }
                                                                                                    );

                                                                                                    return router;

}

} // namespace elizaos
