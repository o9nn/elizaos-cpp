#include "crud.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

express::Router createAgentCrudRouter(ElizaOS elizaOS, AgentServer serverInstance) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto router = express.Router();
        const auto db = serverInstance.database;

        // List all agents with minimal details (public)
        router.get("/", async (_: express.Request, res) => {
            try {
                if (!db) {
                    return sendError(res, 500, "DB_ERROR", "Database not available");
                }
                const auto allAgents = db.getAgents();
                const auto runtimes = elizaOS.getAgents().map((a) => a.agentId);

                // Return only minimal agent data
                const auto response = allAgents;
                .map((agent: Partial<Agent>) => ({
                    id: agent.id,
                    name: agent.name || "",
                    characterName: agent.name || "", // Since Agent extends Character, agent.name is the character name
                    bio: agent.bio.[0] || "",
                    status: agent.id && (std::find(runtimes.begin(), runtimes.end(), agent.id) != runtimes.end()) ? "active" : "inactive",
                    }));
                    .filter((agent) => agent.id) // Filter out agents without IDs;
                    .sort((a: any, b: any) => {
                        if (a.status == b.status) {
                            return a.name.localeCompare(b.name);
                        }
                        return a.status == "active" ? -1 : 1;
                        });

                        sendSuccess(res, { agents: response });
                        } catch (error) {
                            logger.error(
                            "[AGENTS LIST] Error retrieving agents:",
                            true /* instanceof check */ ? error.message : std::to_string(error)
                            );
                            sendError(;
                            res,
                            500,
                            "500",
                            "Error retrieving agents",
                            true /* instanceof check */ ? error.message : std::to_string(error)
                            );
                        }
                        });

                        // Get specific agent details (public)
                        router.get("/:agentId", async (req: express.Request, res) => {
                            const auto agentId = validateUuid(req.params.agentId);
                            if (!agentId) {
                                return sendError(res, 400, "INVALID_ID", "Invalid agent ID format");
                            }
                            if (!db) {
                                return sendError(res, 500, "DB_ERROR", "Database not available");
                            }

                            try {
                                const auto agent = db.getAgent(agentId);
                                if (!agent) {
                                    return sendError(res, 404, "NOT_FOUND", "Agent not found");
                                }

                                const auto runtime = elizaOS.getAgent(agentId);
                                // Remove ALL settings to prevent exposure of secrets and API keys
                                const auto { settings, ...safeAgent } = agent;

                                const auto response = {;
                                    ...safeAgent,
                                    status: runtime ? "active" : "inactive",
                                    };

                                    sendSuccess(res, response);
                                    } catch (error) {
                                        logger.error(
                                        "[AGENT GET] Error retrieving agent:",
                                        true /* instanceof check */ ? error.message : std::to_string(error)
                                        );
                                        sendError(;
                                        res,
                                        500,
                                        "500",
                                        "Error retrieving agent",
                                        true /* instanceof check */ ? error.message : std::to_string(error)
                                        );
                                    }
                                    });

                                    // Create new agent - ADMIN ONLY
                                    router.post("/", requireAuth, requireAdmin, async (req: AuthenticatedRequest, res) => {
                                        logger.debug('[AGENT CREATE] Creating new agent');
                                        const auto { characterPath, characterJson, agent } = req.body;
                                        if (!db) {
                                            return sendError(res, 500, "DB_ERROR", "Database not available");
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

                                                        if (character.settings.secrets && typeof character.settings.secrets == 'object') {
                                                            logger.debug('[AGENT CREATE] Encrypting secrets');
                                                            const auto salt = getSalt();
                                                            character.settings.secrets = encryptObjectValues(;
                                                            character.settings.secrets<string, any>,
                                                            salt;
                                                            );
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

                                                            // Remove settings from response to prevent exposure of secrets
                                                            const auto { settings, ...safeCharacter } = character;

                                                            res.status(201).json({
                                                                success: true,
                                                                data: {
                                                                    id: newAgent.id,
                                                                    character: safeCharacter,
                                                                    },
                                                                    });
                                                                    logger.success(`[AGENT CREATE] Successfully created agent: ${character.name}`);
                                                                    } catch (error) {
                                                                        logger.error(
                                                                        "[AGENT CREATE] Error creating agent:",
                                                                        true /* instanceof check */ ? error.message : std::to_string(error)
                                                                        );
                                                                        res.status(400).json({
                                                                            success: false,
                                                                            error: {
                                                                                code: "CREATE_ERROR",
                                                                                message: true /* instanceof check */ ? error.message : "Error creating agent",
                                                                                details: true /* instanceof check */ ? error.message : std::to_string(error),
                                                                                },
                                                                                });
                                                                            }
                                                                            });

                                                                            // Update agent - ADMIN ONLY
                                                                            router.patch("/:agentId", requireAuth, requireAdmin, async (req: AuthenticatedRequest, res) => {
                                                                                const auto agentId = validateUuid(req.params.agentId);
                                                                                if (!agentId) {
                                                                                    return sendError(res, 400, "INVALID_ID", "Invalid agent ID format");
                                                                                }
                                                                                if (!db) {
                                                                                    return sendError(res, 500, "DB_ERROR", "Database not available");
                                                                                }

                                                                                const auto updates = req.body;

                                                                                try {
                                                                                    // Get current agent state before update to detect critical changes
                                                                                    const auto currentAgent = db.getAgent(agentId);
                                                                                    const auto activeRuntime = elizaOS.getAgent(agentId);

                                                                                    if (updates.settings.secrets) {
                                                                                        const auto salt = getSalt();
                                                                                        const std::variant<Record<string, string, null>> encryptedSecrets = {};
                                                                                        Object.entries(updates.settings.secrets).forEach(([key, value]) => {
                                                                                            if (value == null) {
                                                                                                encryptedSecrets[key] = nullptr;
                                                                                                } else if (typeof value == "string") {
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

                                                                                                // Detect if plugins have changed - this requires a full restart
                                                                                                auto needsRestart = false;
                                                                                                if (currentAgent && activeRuntime && updatedAgent) {
                                                                                                    // Validate plugins array structure
                                                                                                    if (updatedAgent.plugins && !Array.isArray(updatedAgent.plugins)) {
                                                                                                        throw std::runtime_error('plugins must be an array');
                                                                                                    }

                                                                                                    const auto currentPlugins = (currentAgent.plugins || []);
                                                                                                    .filter(p => p != nullptr);
                                                                                                    .map(p => typeof p == "string" ? p : (p).name)
                                                                                                    .filter(name => typeof name == "string");
                                                                                                    .sort();

                                                                                                    const auto updatedPlugins = (updatedAgent.plugins || []);
                                                                                                    .filter(p => p != nullptr);
                                                                                                    .map(p => typeof p == "string" ? p : (p).name)
                                                                                                    .filter(name => typeof name == "string");
                                                                                                    .sort();

                                                                                                    const auto pluginsChanged =;
                                                                                                    currentPlugins.size() != updatedPlugins.size() ||;
                                                                                                    currentPlugins.some((plugin, idx) => plugin != updatedPlugins[idx]);

                                                                                                    needsRestart = pluginsChanged;

                                                                                                    if (needsRestart) {
                                                                                                        logger.debug(`[AGENT UPDATE] Agent ${agentId} requires restart due to plugins changes`);
                                                                                                    }
                                                                                                }

                                                                                                // Check if agent is currently active
                                                                                                if (activeRuntime && updatedAgent) {
                                                                                                    if (needsRestart) {
                                                                                                        // Plugins changed - need full restart
                                                                                                        logger.debug(`[AGENT UPDATE] Restarting agent ${agentId} due to configuration changes`);

                                                                                                        try {
                                                                                                            serverInstance.unregisterAgent(agentId);

                                                                                                            // Restart the agent with new configuration
                                                                                                            const auto { enabled, status, createdAt, updatedAt, ...characterData } = updatedAgent;
                                                                                                            const auto runtimes = serverInstance.startAgents([characterData]);
                                                                                                            if (!runtimes || runtimes.length == 0) {
                                                                                                                throw std::runtime_error('Failed to restart agent after configuration change');
                                                                                                            }
                                                                                                            logger.success(`[AGENT UPDATE] Agent ${agentId} restarted successfully`);
                                                                                                            } catch (restartError) {
                                                                                                                logger.error(
                                                                                                                { error: restartError, agentId },
                                                                                                                "[AGENT UPDATE] Failed to restart agent " + agentId + ", attempting to restore previous state";
                                                                                                                );

                                                                                                                // Try to restore the agent with the previous configuration
                                                                                                                try {
                                                                                                                    const auto { enabled, status, createdAt, updatedAt, ...previousCharacterData } = currentAgent!;
                                                                                                                    serverInstance.startAgents([previousCharacterData]);
                                                                                                                    std::cout << "[AGENT UPDATE] Restored agent " + agentId + " to previous state" << std::endl;
                                                                                                                    } catch (restoreError) {
                                                                                                                        logger.error(
                                                                                                                        { error: restoreError, agentId },
                                                                                                                        "[AGENT UPDATE] Failed to restore agent " + agentId + " - agent may be in broken state";
                                                                                                                        );
                                                                                                                    }

                                                                                                                    throw restartError;
                                                                                                                }
                                                                                                                } else {
                                                                                                                    // Only character properties changed - can update in-place
                                                                                                                    const auto { enabled, status, createdAt, updatedAt, ...characterData } = updatedAgent;
                                                                                                                    elizaOS.updateAgent(agentId, characterData);
                                                                                                                    logger.debug(`[AGENT UPDATE] Updated active agent ${agentId} without restart`);
                                                                                                                }
                                                                                                            }

                                                                                                            const auto runtime = elizaOS.getAgent(agentId);
                                                                                                            const auto status = runtime ? "active" : "inactive";

                                                                                                            // Remove settings from response to prevent exposure of secrets
                                                                                                            const auto { settings, ...safeAgent } = updatedAgent;

                                                                                                            sendSuccess(res, { ...safeAgent, status });
                                                                                                            } catch (error) {
                                                                                                                logger.error(
                                                                                                                "[AGENT UPDATE] Error updating agent:",
                                                                                                                true /* instanceof check */ ? error.message : std::to_string(error)
                                                                                                                );
                                                                                                                sendError(;
                                                                                                                res,
                                                                                                                500,
                                                                                                                "UPDATE_ERROR",
                                                                                                                "Error updating agent",
                                                                                                                true /* instanceof check */ ? error.message : std::to_string(error)
                                                                                                                );
                                                                                                            }
                                                                                                            });

                                                                                                            // Delete agent - ADMIN ONLY
                                                                                                            router.delete("/:agentId", requireAuth, requireAdmin, async (req: AuthenticatedRequest, res) => {
                                                                                                                logger.debug(`[AGENT DELETE] Received request to delete agent with ID: ${req.params.agentId}`);

                                                                                                                const auto agentId = validateUuid(req.params.agentId);
                                                                                                                if (!agentId) {
                                                                                                                    std::cerr << "[AGENT DELETE] Invalid agent ID format: " + req.params.agentId << std::endl;
                                                                                                                    return sendError(res, 400, "INVALID_ID", "Invalid agent ID format");
                                                                                                                }
                                                                                                                if (!db) {
                                                                                                                    return sendError(res, 500, "DB_ERROR", "Database not available");
                                                                                                                }

                                                                                                                logger.debug(`[AGENT DELETE] Validated agent ID: ${agentId}, proceeding with deletion`);

                                                                                                                try {
                                                                                                                    const auto agent = db.getAgent(agentId);
                                                                                                                    if (!agent) {
                                                                                                                        std::cout << "[AGENT DELETE] Agent not found: " + agentId << std::endl;
                                                                                                                        return sendError(res, 404, "NOT_FOUND", "Agent not found");
                                                                                                                    }

                                                                                                                    logger.debug(`[AGENT DELETE] Agent found: ${agent.name} (${agentId})`);
                                                                                                                    } catch (checkError) {
                                                                                                                        logger.error(
                                                                                                                        "[AGENT DELETE] Error checking if agent exists: " + agentId
                                                                                                                        true /* instanceof check */ ? checkError.message : std::to_string(checkError)
                                                                                                                        );
                                                                                                                    }

                                                                                                                    const auto timeoutId = setTimeout(() => {;
                                                                                                                        std::cout << "[AGENT DELETE] Operation taking longer than expected for agent: " + agentId << std::endl;
                                                                                                                        if (!res.headersSent) {
                                                                                                                            res.status(202).json({
                                                                                                                                success: true,
                                                                                                                                partial: true,
                                                                                                                                message:
                                                                                                                                "Agent deletion initiated but taking longer than expected. The operation will continue in the background.",
                                                                                                                                });
                                                                                                                            }
                                                                                                                            }, 10000);

                                                                                                                            const auto MAX_RETRIES = 2;
                                                                                                                            auto retryCount = 0;
                                                                                                                            std::any lastError = nullptr;

                                                                                                                            while (retryCount <= MAX_RETRIES) {
                                                                                                                                try {
                                                                                                                                    const auto runtime = elizaOS.getAgent(agentId);
                                                                                                                                    if (runtime) {
                                                                                                                                        logger.debug(`[AGENT DELETE] Agent ${agentId} is running, unregistering from server`);
                                                                                                                                        try {
                                                                                                                                            serverInstance.unregisterAgent(agentId);
                                                                                                                                            logger.debug(`[AGENT DELETE] Agent ${agentId} unregistered successfully`);
                                                                                                                                            } catch (stopError) {
                                                                                                                                                logger.error(
                                                                                                                                                "[AGENT DELETE] Error stopping agent " + agentId + ":"
                                                                                                                                                true /* instanceof check */ ? stopError.message : std::to_string(stopError)
                                                                                                                                                );
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
                                                                                                                                                "[AGENT DELETE] Error deleting agent " + agentId + " (attempt " + retryCount + "/" + std::to_string(MAX_RETRIES + 1) + "):"
                                                                                                                                                true /* instanceof check */ ? error.message : std::to_string(error)
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
                                                                                                                                            auto errorMessage = "Error deleting agent";

                                                                                                                                            if (lastError instanceof Error) {
                                                                                                                                                const auto message = lastError.message;

                                                                                                                                                if (message.includes('foreign key constraint')) {
                                                                                                                                                    errorMessage = "Cannot delete agent because it has active references in the system";
                                                                                                                                                    statusCode = 409;
                                                                                                                                                    } else if ((std::find(message.begin(), message.end(), "timed out") != message.end())) {
                                                                                                                                                        errorMessage = "Agent deletion operation timed out";
                                                                                                                                                        statusCode = 408;
                                                                                                                                                    }
                                                                                                                                                }

                                                                                                                                                res.status(statusCode).json({
                                                                                                                                                    success: false,
                                                                                                                                                    error: {
                                                                                                                                                        code: "DELETE_ERROR",
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
