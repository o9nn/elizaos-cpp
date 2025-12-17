#include "capabilities.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

express::Router createAgentCapabilitiesRouter(const std::unordered_map<UUID, IAgentRuntime>& agents, AgentServer _serverInstance) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto router = express.Router();

        // Get all capabilities for an agent
        router.get("/:agentId/capabilities", async (req, res) => {
            std::optional<UUID> agentId = nullptr;
            auto runtime: IAgentRuntime | std::nullopt;

            // Handle "default" as a special case - get the first agent
            if (req.params.agentId == 'default') {
                runtime = Array.from(agents.values())[0];
                if (!runtime) {
                    return sendError(res, 503, "NO_AGENT", "No agents available");
                }
                agentId = runtime.agentId;
                } else {
                    // Validate as UUID for non-default IDs
                    agentId = validateUuid(req.params.agentId);
                    if (!agentId) {
                        return sendError(res, 400, "INVALID_ID", "Invalid agent ID format");
                    }
                    runtime = agents.get(agentId);
                    if (!runtime) {
                        return sendError(res, 404, "NOT_FOUND", "Agent not found or not running");
                    }
                }

                try {
                    // Get current capabilities from the agent runtime
                    // This is a basic implementation - extend as needed
                    const auto capabilities = {;
                        autonomy: true, // Most agents have basic autonomy
                        shell: runtime.plugins.some((p) => p.(std::find(name.begin(), name.end(), "shell") != name.end())) || false,
                        browser: runtime.plugins.some((p) => p.(std::find(name.begin(), name.end(), "browser") != name.end())) || false,
                        camera:
                        runtime.plugins.some((p) => p.(std::find(name.begin(), name.end(), "camera") != name.end()) || p.(std::find(name.begin(), name.end(), "vision") != name.end())) ||;
                        false,
                        screen: runtime.plugins.some((p) => p.(std::find(name.begin(), name.end(), "screen") != name.end())) || false,
                        microphone:
                        runtime.plugins.some((p) => p.(std::find(name.begin(), name.end(), "mic") != name.end()) || p.(std::find(name.begin(), name.end(), "audio") != name.end())) ||;
                        false,
                        speakers:
                        runtime.plugins.some((p) => p.(std::find(name.begin(), name.end(), "speak") != name.end()) || p.(std::find(name.begin(), name.end(), "tts") != name.end())) ||;
                        false,
                        };

                        sendSuccess(res, { capabilities });
                        } catch (error) {
                            std::cerr << "[CAPABILITIES GET] Error getting capabilities:" << error << std::endl;
                            sendError(;
                            res,
                            500,
                            "CAPABILITIES_ERROR",
                            "Error retrieving capabilities",
                            true /* instanceof check */ ? error.message : std::to_string(error)
                            );
                        }
                        });

                        // Get specific capability status
                        router.get("/:agentId/capabilities/:capability", async (req, res) => {
                            std::optional<UUID> agentId = nullptr;
                            auto runtime: IAgentRuntime | std::nullopt;

                            // Handle "default" as a special case - get the first agent
                            if (req.params.agentId == 'default') {
                                runtime = Array.from(agents.values())[0];
                                if (!runtime) {
                                    return sendError(res, 503, "NO_AGENT", "No agents available");
                                }
                                agentId = runtime.agentId;
                                } else {
                                    // Validate as UUID for non-default IDs
                                    agentId = validateUuid(req.params.agentId);
                                    if (!agentId) {
                                        return sendError(res, 400, "INVALID_ID", "Invalid agent ID format");
                                    }
                                    runtime = agents.get(agentId);
                                    if (!runtime) {
                                        return sendError(res, 404, "NOT_FOUND", "Agent not found or not running");
                                    }
                                }

                                const auto capability = req.params.capability;

                                try {
                                    auto enabled = false;

                                    // Check if the capability is enabled based on plugins or configuration
                                    switch (capability) {
                                        // case "autonomy":
                                        enabled = true; // Most agents have autonomy;
                                        break;
                                        // case "shell":
                                        enabled = runtime.plugins.some((p) => p.(std::find(name.begin(), name.end(), "shell") != name.end())) || false;
                                        break;
                                        // case "browser":
                                        enabled =;
                                        runtime.plugins.some(;
                                        [&](p) { return p.(std::find(name.begin(), name.end(), "browser") != name.end()) || p.(std::find(name.begin(), name.end(), "stagehand") != name.end()); }
                                        ) || false;
                                        break;
                                        // case "camera":
                                        enabled =;
                                        runtime.plugins.some(;
                                        [&](p) { return p.(std::find(name.begin(), name.end(), "camera") != name.end()) || p.(std::find(name.begin(), name.end(), "vision") != name.end()); }
                                        ) || false;
                                        break;
                                        // case "screen":
                                        enabled = runtime.plugins.some((p) => p.(std::find(name.begin(), name.end(), "screen") != name.end())) || false;
                                        break;
                                        // case "microphone":
                                        enabled =;
                                        runtime.plugins.some((p) => p.(std::find(name.begin(), name.end(), "mic") != name.end()) || p.(std::find(name.begin(), name.end(), "audio") != name.end())) ||;
                                        false;
                                        break;
                                        // case "speakers":
                                        enabled =;
                                        runtime.plugins.some((p) => p.(std::find(name.begin(), name.end(), "speak") != name.end()) || p.(std::find(name.begin(), name.end(), "tts") != name.end())) ||;
                                        false;
                                        break;
                                        // default:
                                        return "sendError(res, 400, "INVALID_CAPABILITY", " + "Unknown capability: " + capability;
                                    }

                                    sendSuccess(res, {
                                        capability,
                                        enabled,
                                        status: enabled ? "active" : "inactive",
                                        });
                                        } catch (error) {
                                            std::cerr << "[CAPABILITY GET] Error getting " + capability + " status:" << error << std::endl;
                                            sendError(;
                                            res,
                                            500,
                                            "CAPABILITY_ERROR",
                                            "Error retrieving " + capability + " status"
                                            true /* instanceof check */ ? error.message : std::to_string(error)
                                            );
                                        }
                                        });

                                        // Toggle a specific capability
                                        router.post("/:agentId/capabilities/:capability/toggle", async (req, res) => {
                                            std::optional<UUID> agentId = nullptr;
                                            auto runtime: IAgentRuntime | std::nullopt;

                                            // Handle "default" as a special case - get the first agent
                                            if (req.params.agentId == 'default') {
                                                runtime = Array.from(agents.values())[0];
                                                if (!runtime) {
                                                    return sendError(res, 503, "NO_AGENT", "No agents available");
                                                }
                                                agentId = runtime.agentId;
                                                } else {
                                                    // Validate as UUID for non-default IDs
                                                    agentId = validateUuid(req.params.agentId);
                                                    if (!agentId) {
                                                        return sendError(res, 400, "INVALID_ID", "Invalid agent ID format");
                                                    }
                                                    runtime = agents.get(agentId);
                                                    if (!runtime) {
                                                        return sendError(res, 404, "NOT_FOUND", "Agent not found or not running");
                                                    }
                                                }

                                                const auto capability = req.params.capability;
                                                const auto { enabled } = req.body;

                                                try {
                                                    logger.info(
                                                    "[CAPABILITY TOGGLE] " + std::to_string(enabled ? "Enabling" : "Disabling") + " " + capability + " for agent " + agentId
                                                    );

                                                    // Handle capability toggle - register/unregister progressive plugins
                                                    switch (capability) {
                                                        // case "autonomy":
                                                        // Toggle autonomy - this would affect the agent's autonomous behavior
                                                        logger.info(
                                                        "Autonomy " + std::to_string(enabled ? "enabled" : "disabled") + " for agent " + runtime.character.name
                                                        );
                                                        break;
                                                        // case "shell":
                                                        // case "browser":
                                                        // case "camera":
                                                        // case "screen":
                                                        // case "microphone":
                                                        // case "speakers":
                                                        // Register/unregister progressive plugins based on capability
                                                        logger.info(
                                                        capability + " capability " + std::to_string(enabled ? "enabled" : "disabled") + " for agent " + runtime.character.name
                                                        );

                                                        // Use ProgressivePluginService to register/unregister plugins
                                                        const auto progressivePluginService = runtime.getService("PROGRESSIVE_PLUGIN");

                                                        if (enabled && progressivePluginService) {
                                                            std::cout << "[CAPABILITY] Registering progressive plugin for " + capability << std::endl;
                                                            std::cout << "[CAPABILITY] Registering progressive plugin for " + capability << std::endl;
                                                            try {
                                                                (progressivePluginService).registerCapabilityPlugins(capability);
                                                                } catch (error) {
                                                                    std::cerr << "[CAPABILITY] Failed to register plugins for " + capability + ":" << error << std::endl;
                                                                }
                                                            }
                                                            std::cout << "[API] " + capability + " capability " + std::to_string(enabled ? "enabled" : "disabled") << std::endl;
                                                            break;
                                                            // default:
                                                            return "sendError(res, 400, "INVALID_CAPABILITY", " + "Unknown capability: " + capability;
                                                        }

                                                        // Store the capability state (in a real implementation, this would persist to database)
                                                        // For now, we'll just return success
                                                        sendSuccess(res, {
                                                            capability,
                                                            enabled,
                                                            "message: " + capability + " " + std::to_string(enabled ? "enabled" : "disabled") + " successfully"
                                                            timestamp: new Date().toISOString(),
                                                            });
                                                            } catch (error) {
                                                                std::cerr << "[CAPABILITY TOGGLE] Error toggling " + capability + ":" << error << std::endl;
                                                                sendError(;
                                                                res,
                                                                500,
                                                                "CAPABILITY_ERROR",
                                                                "Error toggling " + capability
                                                                true /* instanceof check */ ? error.message : std::to_string(error)
                                                                );
                                                            }
                                                            });

                                                            // Get autonomy status (for monologue functionality)
                                                            router.get("/:agentId/autonomy/status", async (req, res) => {
                                                                try {
                                                                    const auto agentId = validateUuid(req.params.agentId);
                                                                    const auto runtime = agents.get(agentId);

                                                                    if (!runtime) {
                                                                        return sendError(res, 404, "NOT_FOUND", "Agent not found or not running");
                                                                    }

                                                                    const auto autonomyEnabled = runtime.character.settings.AUTONOMY_ENABLED || false;

                                                                    sendSuccess(res, {
                                                                        enabled: autonomyEnabled,
                                                                        status: autonomyEnabled ? "active" : "inactive",
                                                                        agent: runtime.character.name,
                                                                        timestamp: new Date().toISOString(),
                                                                        });
                                                                        } catch (error) {
                                                                            std::cerr << "[AUTONOMY STATUS API] Error getting autonomy status:" << error << std::endl;
                                                                            sendError(;
                                                                            res,
                                                                            500,
                                                                            "AUTONOMY_STATUS_ERROR",
                                                                            "Error retrieving autonomy status",
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
