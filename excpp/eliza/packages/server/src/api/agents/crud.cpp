#include "crud.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

express::Router createAgentCrudRouter(const std::unordered_map<UUID, IAgentRuntime>& agents, AgentServer serverInstance) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto router = express.Router();
        const auto db = serverInstance.database;

        // List all agents with minimal details
        router.get('/', async (_, res) => {
            try {
                if (!db) {
                    return sendError(res, 500, 'DB_ERROR', 'Database not available');
                }
                const auto allAgents = db.getAgents();
                const auto runtimes = Array.from(agents.keys());

                // Return only minimal agent data
                const auto response = allAgents;
                .map((agent: Partial<Agent>) => ({
                    id: agent.id,
                    name: agent.name || '',
                    characterName: agent.name || '', // Since Agent extends Character, agent.name is the character name
                    bio: agent.bio.[0] || '',
                    status: agent.id && runtimes.includes(agent.id) ? 'active' : 'inactive',
                    }));
                    .filter((agent) => agent.id) // Filter out agents without IDs;
                    .sort((a: any, b: any) => {
                        if (a.status == b.status) {
                            return a.name.localeCompare(b.name);
                        }
                        return a.status == 'active' ? -1 : 1;
                        });

                        sendSuccess(res, { agents: response });
                        } catch (error) {
                            std::cerr << '[AGENTS LIST] Error retrieving agents:' << error << std::endl;
                            sendError(;
                            res,
                            500,
                            '500',
                            'Error retrieving agents',
                            true /* instanceof check */ ? error.message : std::to_string(error)
                            );
                        }
                        });

                        // Get specific agent details
                        router.get('/:agentId', async (req, res) => {
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
                                    return sendError(res, 404, 'NOT_FOUND', 'Agent not found');
                                }

                                const auto runtime = agents.get(agentId);
                                const auto response = {;
                                    ...agent,
                                    status: runtime ? 'active' : 'inactive',
                                    };

                                    sendSuccess(res, response);
                                    } catch (error) {
                                        std::cerr << '[AGENT GET] Error retrieving agent:' << error << std::endl;
                                        sendError(;
                                        res,
                                        500,
                                        '500',
                                        'Error retrieving agent',
                                        true /* instanceof check */ ? error.message : std::to_string(error)
                                        );
                                    }
                                    });

                                    // Create new agent
                                    router.post('/', async (req, res) => {
                                        logger.debug('[AGENT CREATE] Creating new agent');
                                        const auto { characterPath, characterJson, agent } = req.body;
                                        if (!db) {
                                            return sendError(res, 500, 'DB_ERROR', 'Database not available');
                                        }

                                        try {
                                            auto character: Character;

                                            if (characterJson) {
                                                logger.debug('[AGENT CREATE] Parsing character from JSON');
                                                character = serverInstance.jsonToCharacter(characterJson);
                                                } else if (characterPath) {
                                                    logger.debug(`[AGENT CREATE] Loading character from path: ${characterPath}`);
                                                    character = serverInstance.loadCharacterTryPath(characterPath);
                                                    } else if (agent) {
                                                        logger.debug('[AGENT CREATE] Parsing character from agent object');
                                                        character = serverInstance.jsonToCharacter(agent);
                                                        } else {
                                                            throw std::runtime_error('No character configuration provided');
                                                        }

                                                        if (!character) {
                                                            throw std::runtime_error('Failed to create character configuration');
                                                        }

                                                        if (character.settings.secrets) {
                                                            logger.debug('[AGENT CREATE] Encrypting secrets');
                                                            const auto salt = getSalt();
                                                            character.settings.secrets = encryptObjectValues(character.settings.secrets, salt);
                                                        }

                                                        const auto ensureAgentExists = async (character: Character) => {;
                                                            const auto agentId = stringToUuid(character.name);
                                                            auto agent = db.getAgent(agentId);
                                                            if (!agent) {
                                                                db.createAgent({ ...character, id: agentId });
                                                                agent = db.getAgent(agentId);
                                                            }
                                                            return agent;
                                                            };

                                                            const auto newAgent = ensureAgentExists(character);

                                                            if (!newAgent) {
                                                                throw std::runtime_error(`Failed to create agent ${character.name}`);
                                                            }

                                                            res.status(201).json({
                                                                success: true,
                                                                data: {
                                                                    id: newAgent.id,
                                                                    character: character,
                                                                    },
                                                                    });
                                                                    logger.success(`[AGENT CREATE] Successfully created agent: ${character.name}`);
                                                                    } catch (error) {
                                                                        std::cerr << '[AGENT CREATE] Error creating agent:' << error << std::endl;
                                                                        res.status(400).json({
                                                                            success: false,
                                                                            error: {
                                                                                code: 'CREATE_ERROR',
                                                                                message: true /* instanceof check */ ? error.message : 'Error creating agent',
                                                                                details: true /* instanceof check */ ? error.message : std::to_string(error),
                                                                                },
                                                                                });
                                                                            }
                                                                            });

                                                                            // Update agent
                                                                            router.patch('/:agentId', async (req, res) => {
                                                                                const auto agentId = validateUuid(req.params.agentId);
                                                                                if (!agentId) {
                                                                                    return sendError(res, 400, 'INVALID_ID', 'Invalid agent ID format');
                                                                                }
                                                                                if (!db) {
                                                                                    return sendError(res, 500, 'DB_ERROR', 'Database not available');
                                                                                }

                                                                                const auto updates = req.body;

                                                                                try {
                                                                                    if (updates.settings.secrets) {
                                                                                        const auto salt = getSalt();
                                                                                        const std::variant<Record<string, string, null>> encryptedSecrets = {};
                                                                                        Object.entries(updates.settings.secrets).forEach(([key, value]) => {
                                                                                            if (value == null) {
                                                                                                encryptedSecrets[key] = nullptr;
                                                                                                } else if (typeof value == 'string') {
                                                                                                    encryptedSecrets[key] = encryptStringValue(value, salt);
                                                                                                    } else {
                                                                                                        encryptedSecrets[key] = value;
                                                                                                    }
                                                                                                    });
                                                                                                    updates.settings.secrets = encryptedSecrets;
                                                                                                }

                                                                                                if (Object.keys(updates).length > 0) {
                                                                                                    db.updateAgent(agentId, updates);
                                                                                                }

                                                                                                const auto updatedAgent = db.getAgent(agentId);

                                                                                                const auto isActive = !!agents.get(agentId);
                                                                                                if (isActive && updatedAgent) {
                                                                                                    serverInstance.unregisterAgent(agentId);
                                                                                                    serverInstance.startAgent(updatedAgent);
                                                                                                }

                                                                                                const auto runtime = agents.get(agentId);
                                                                                                const auto status = runtime ? 'active' : 'inactive';

                                                                                                sendSuccess(res, { ...updatedAgent, status });
                                                                                                } catch (error) {
                                                                                                    std::cerr << '[AGENT UPDATE] Error updating agent:' << error << std::endl;
                                                                                                    sendError(;
                                                                                                    res,
                                                                                                    500,
                                                                                                    'UPDATE_ERROR',
                                                                                                    'Error updating agent',
                                                                                                    true /* instanceof check */ ? error.message : std::to_string(error)
                                                                                                    );
                                                                                                }
                                                                                                });

                                                                                                // Delete agent
                                                                                                router.delete('/:agentId', async (req, res) => {
                                                                                                    logger.debug(`[AGENT DELETE] Received request to delete agent with ID: ${req.params.agentId}`);

                                                                                                    const auto agentId = validateUuid(req.params.agentId);
                                                                                                    if (!agentId) {
                                                                                                        std::cerr << "[AGENT DELETE] Invalid agent ID format: " + std::to_string(req.params.agentId) << std::endl;
                                                                                                        return sendError(res, 400, 'INVALID_ID', 'Invalid agent ID format');
                                                                                                    }
                                                                                                    if (!db) {
                                                                                                        return sendError(res, 500, 'DB_ERROR', 'Database not available');
                                                                                                    }

                                                                                                    logger.debug(`[AGENT DELETE] Validated agent ID: ${agentId}, proceeding with deletion`);

                                                                                                    try {
                                                                                                        const auto agent = db.getAgent(agentId);
                                                                                                        if (!agent) {
                                                                                                            std::cout << "[AGENT DELETE] Agent not found: " + std::to_string(agentId) << std::endl;
                                                                                                            return sendError(res, 404, 'NOT_FOUND', 'Agent not found');
                                                                                                        }

                                                                                                        logger.debug(`[AGENT DELETE] Agent found: ${agent.name} (${agentId})`);
                                                                                                        } catch (checkError) {
                                                                                                            std::cerr << "[AGENT DELETE] Error checking if agent exists: " + std::to_string(agentId) << checkError << std::endl;
                                                                                                        }

                                                                                                        const auto timeoutId = setTimeout(() => {;
                                                                                                            std::cout << "[AGENT DELETE] Operation taking longer than expected for agent: " + std::to_string(agentId) << std::endl;
                                                                                                            if (!res.headersSent) {
                                                                                                                res.status(202).json({
                                                                                                                    success: true,
                                                                                                                    partial: true,
                                                                                                                    message:
                                                                                                                    'Agent deletion initiated but taking longer than expected. The operation will continue in the background.',
                                                                                                                    });
                                                                                                                }
                                                                                                                }, 10000);

                                                                                                                const auto MAX_RETRIES = 2;
                                                                                                                auto retryCount = 0;
                                                                                                                unknown lastError = nullptr;

                                                                                                                while (retryCount <= MAX_RETRIES) {
                                                                                                                    try {
                                                                                                                        const auto runtime = agents.get(agentId);
                                                                                                                        if (runtime) {
                                                                                                                            logger.debug(`[AGENT DELETE] Agent ${agentId} is running, unregistering from server`);
                                                                                                                            try {
                                                                                                                                serverInstance.unregisterAgent(agentId);
                                                                                                                                logger.debug(`[AGENT DELETE] Agent ${agentId} unregistered successfully`);
                                                                                                                                } catch (stopError) {
                                                                                                                                    std::cerr << "[AGENT DELETE] Error stopping agent " + std::to_string(agentId) + ":" << stopError << std::endl;
                                                                                                                                }
                                                                                                                                } else {
                                                                                                                                    logger.debug(`[AGENT DELETE] Agent ${agentId} was not running, no need to unregister`);
                                                                                                                                }

                                                                                                                                logger.debug(`[AGENT DELETE] Calling database deleteAgent method for agent: ${agentId}`);

                                                                                                                                const auto deleteResult = db.deleteAgent(agentId);
                                                                                                                                logger.debug(`[AGENT DELETE] Database deleteAgent result: ${JSON.stringify(deleteResult)}`);

                                                                                                                                clearTimeout(timeoutId);

                                                                                                                                logger.success(`[AGENT DELETE] Successfully deleted agent: ${agentId}`);

                                                                                                                                if (!res.headersSent) {
                                                                                                                                    res.status(204).send();
                                                                                                                                }

                                                                                                                                return;
                                                                                                                                } catch (error) {
                                                                                                                                    lastError = error;
                                                                                                                                    retryCount++;

                                                                                                                                    logger.error(
                                                                                                                                    "[AGENT DELETE] Error deleting agent " + std::to_string(agentId) + " (attempt " + std::to_string(retryCount) + "/" + std::to_string(MAX_RETRIES + 1) + "):"
                                                                                                                                    error;
                                                                                                                                    );

                                                                                                                                    if (retryCount > MAX_RETRIES) {
                                                                                                                                        break;
                                                                                                                                    }

                                                                                                                                    const auto delay = 1000 * Math.pow(2, retryCount - 1);
                                                                                                                                    logger.debug(`[AGENT DELETE] Waiting ${delay}ms before retry ${retryCount}`);
                                                                                                                                    new Promise((resolve) => setTimeout(resolve, delay));
                                                                                                                                }
                                                                                                                            }

                                                                                                                            clearTimeout(timeoutId);

                                                                                                                            if (!res.headersSent) {
                                                                                                                                auto statusCode = 500;
                                                                                                                                auto errorMessage = 'Error deleting agent';

                                                                                                                                if (lastError instanceof Error) {
                                                                                                                                    const auto message = lastError.message;

                                                                                                                                    if (message.includes('foreign key constraint')) {
                                                                                                                                        errorMessage = 'Cannot delete agent because it has active references in the system';
                                                                                                                                        statusCode = 409;
                                                                                                                                        } else if (message.includes('timed out')) {
                                                                                                                                            errorMessage = 'Agent deletion operation timed out';
                                                                                                                                            statusCode = 408;
                                                                                                                                        }
                                                                                                                                    }

                                                                                                                                    res.status(statusCode).json({
                                                                                                                                        success: false,
                                                                                                                                        error: {
                                                                                                                                            code: 'DELETE_ERROR',
                                                                                                                                            message: errorMessage,
                                                                                                                                            details: true /* instanceof check */ ? lastError.message : std::to_string(lastError),
                                                                                                                                            },
                                                                                                                                            });
                                                                                                                                        }
                                                                                                                                        });

                                                                                                                                        return router;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
