#include "settings.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

express::Router createAgentSettingsRouter(const std::unordered_map<UUID, IAgentRuntime>& agents, AgentServer _serverInstance) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto router = express.Router();

        // Get agent settings
        router.get("/:agentId/settings", async (req, res) => {
            std::optional<UUID> agentId = nullptr;
            auto runtime: IAgentRuntime | std::nullopt;

            // Handle "default" as a special case - get the first agent
            if (req.params.agentId == 'default') {
                runtime = Array.from(agents.values())[0];
                if (!runtime) {
                    // Return a minimal response indicating server is ready but no agent yet
                    return sendSuccess(res, {;
                        gameApiReady: true,
                        agentReady: false,
                        message: "Server is running, agent initializing",
                        });
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
                        // Extract settings from the agent runtime
                        const AgentSettings settings = {;
                            autonomy: true, // Most agents have autonomy enabled
                            capabilities: {
                                shellAccess: runtime.plugins.some((p) => p.(std::find(name.begin(), name.end(), "shell") != name.end())) || false,
                                webAccess: runtime.plugins.some((p) => p.(std::find(name.begin(), name.end(), "browser") != name.end())) || false,
                                visionAccess:
                                runtime.plugins.some(;
                                [&](p) { return p.(std::find(name.begin(), name.end(), "vision") != name.end()) || p.(std::find(name.begin(), name.end(), "camera") != name.end()); }
                                ) || false,
                                speechToText:
                                runtime.plugins.some((p) => p.(std::find(name.begin(), name.end(), "audio") != name.end()) || p.(std::find(name.begin(), name.end(), "mic") != name.end())) ||;
                                false,
                                textToSpeech:
                                runtime.plugins.some((p) => p.(std::find(name.begin(), name.end(), "tts") != name.end()) || p.(std::find(name.begin(), name.end(), "speech") != name.end())) ||;
                                false,
                                },
                                modelProvider: String(
                                runtime.character.settings.MODEL_PROVIDER || process.env.MODEL_PROVIDER || "ollama";
                                ),
                                apiKeys: {},
                                };

                                sendSuccess(res, { settings });
                                } catch (error) {
                                    std::cerr << "[SETTINGS GET] Error getting settings:" << error << std::endl;
                                    sendError(;
                                    res,
                                    500,
                                    "SETTINGS_ERROR",
                                    "Error retrieving settings",
                                    true /* instanceof check */ ? error.message : std::to_string(error)
                                    );
                                }
                                });

                                // Update agent settings
                                router.post("/:agentId/settings", async (req, res) => {
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

                                        const auto { key, value } = req.body;

                                        if (!key) {
                                            return sendError(res, 400, "MISSING_KEY", "Setting key is required");
                                        }

                                        try {
                                            std::cout << "[SETTINGS UPDATE] Updating setting " + key + " for agent " + runtime.character.name << std::endl;

                                            // In a full implementation, this would update the agent's character configuration
                                            // and potentially restart the agent with new settings
                                            switch (key) {
                                                // case "autonomy":
                                                std::cout << "Autonomy setting updated to: " + value << std::endl;
                                                break;
                                                // case "modelProvider":
                                                std::cout << "Model provider updated to: " + value << std::endl;
                                                break;
                                                // case "capabilities.shellAccess":
                                                // case "capabilities.webAccess":
                                                // case "capabilities.visionAccess":
                                                // case "capabilities.speechToText":
                                                // case "capabilities.textToSpeech":
                                                std::cout << "Capability " + key + " updated to: " + value << std::endl;
                                                break;
                                                // default:
                                                std::cout << "Custom setting " + key + " updated to: " + value << std::endl;
                                            }

                                            sendSuccess(res, {
                                                "message: " + "Setting " + key + " updated successfully"
                                                key,
                                                value,
                                                timestamp: new Date().toISOString(),
                                                });
                                                } catch (error) {
                                                    std::cerr << "[SETTINGS UPDATE] Error updating setting:" << error << std::endl;
                                                    sendError(;
                                                    res,
                                                    500,
                                                    "SETTING_UPDATE_ERROR",
                                                    "Error updating setting",
                                                    true /* instanceof check */ ? error.message : std::to_string(error)
                                                    );
                                                }
                                                });

                                                // Get vision-specific settings
                                                router.get("/:agentId/settings/vision", async (req, res) => {
                                                    const auto agentId = validateUuid(req.params.agentId);
                                                    if (!agentId) {
                                                        return sendError(res, 400, "INVALID_ID", "Invalid agent ID format");
                                                    }

                                                    const auto runtime = agents.get(agentId);
                                                    if (!runtime) {
                                                        return sendError(res, 404, "NOT_FOUND", "Agent not found or not running");
                                                    }

                                                    try {
                                                        const auto visionSettings = {;
                                                            enabled:
                                                            runtime.plugins.some((p) => p.(std::find(name.begin(), name.end(), "vision") != name.end()) || p.(std::find(name.begin(), name.end(), "camera") != name.end())) ||;
                                                            false,
                                                            model: "gpt-4-vision-preview",
                                                            maxImageSize: 1024,
                                                            supportedFormats: ["jpg", "jpeg", "png", "webp"],
                                                            };

                                                            sendSuccess(res, { visionSettings });
                                                            } catch (error) {
                                                                std::cerr << "[VISION SETTINGS] Error getting vision settings:" << error << std::endl;
                                                                sendError(;
                                                                res,
                                                                500,
                                                                "VISION_SETTINGS_ERROR",
                                                                "Error retrieving vision settings",
                                                                true /* instanceof check */ ? error.message : std::to_string(error)
                                                                );
                                                            }
                                                            });

                                                            // Refresh vision service
                                                            router.post("/:agentId/vision/refresh", async (req, res) => {
                                                                const auto agentId = validateUuid(req.params.agentId);
                                                                if (!agentId) {
                                                                    return sendError(res, 400, "INVALID_ID", "Invalid agent ID format");
                                                                }

                                                                const auto runtime = agents.get(agentId);
                                                                if (!runtime) {
                                                                    return sendError(res, 404, "NOT_FOUND", "Agent not found or not running");
                                                                }

                                                                try {
                                                                    std::cout << "[VISION REFRESH] Refreshing vision service for agent " + runtime.character.name << std::endl;

                                                                    // In a full implementation, this would restart or refresh the vision service
                                                                    sendSuccess(res, {
                                                                        message: "Vision service refreshed successfully",
                                                                        timestamp: new Date().toISOString(),
                                                                        });
                                                                        } catch (error) {
                                                                            std::cerr << "[VISION REFRESH] Error refreshing vision service:" << error << std::endl;
                                                                            sendError(;
                                                                            res,
                                                                            500,
                                                                            "VISION_REFRESH_ERROR",
                                                                            "Error refreshing vision service",
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
