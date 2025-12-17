#include "plugins.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

express::Router createAgentPluginsRouter(Map<UUID agents, auto IAgentRuntime>, AgentServer _serverInstance) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto router = express.Router();

        // Get plugin configurations for a specific agent
        router.get('/:agentId/plugins', async (req, res) => {
            try {
                const auto agentId = validateUuid(req.params.agentId);
                const auto runtime = agents.get(agentId);

                if (!runtime) {
                    return sendError(res, 404, 'NOT_FOUND', 'Agent not found or not running');
                }

                // Return plugin configurations based on agent's loaded plugins
                const std::vector<PluginConfig> pluginConfigs = [;
                {
                    id: 'bootstrap',
                    name: '@elizaos/plugin-bootstrap',
                    displayName: 'Bootstrap Plugin',
                    enabled: true,
                    version: '1.0.0',
                    description: 'Core bootstrap functionality for ElizaOS agents',
                    config: {},
                    },
                    {
                        id: 'openai',
                        name: '@elizaos/plugin-openai',
                        displayName: 'OpenAI Plugin',
                        enabled: true,
                        version: '1.0.0',
                        description: 'OpenAI integration for language model capabilities',
                        config: {
                            model: runtime.character.settings.model || 'gpt-4o-mini',
                            temperature: 0.7,
                            },
                            },
                            {
                                id: 'sql',
                                name: '@elizaos/plugin-sql',
                                displayName: 'SQL Plugin',
                                enabled: true,
                                version: '1.0.0',
                                description: 'Database integration for persistent memory',
                                config: {},
                                },
                                ];

                                sendSuccess(res, { plugins: pluginConfigs });
                                } catch (error) {
                                    std::cerr << '[PLUGINS API] Error getting plugin configs:' << error << std::endl;
                                    sendError(;
                                    res,
                                    500,
                                    'PLUGINS_ERROR',
                                    'Error retrieving plugin configurations',
                                    true /* instanceof check */ ? error.message : std::to_string(error)
                                    );
                                }
                                });

                                // Update plugin configuration
                                router.post('/:agentId/plugins/:pluginId/config', async (req, res) => {
                                    try {
                                        const auto agentId = validateUuid(req.params.agentId);
                                        const auto pluginId = req.params.pluginId;
                                        const auto { config } = req.body;

                                        const auto runtime = agents.get(agentId);
                                        if (!runtime) {
                                            return sendError(res, 404, 'NOT_FOUND', 'Agent not found or not running');
                                        }

                                        logger.info(
                                        "[PLUGINS API] Updated config for plugin " + std::to_string(pluginId) + " on agent " + std::to_string(runtime.character.name);
                                        );
                                        sendSuccess(res, {
                                            "Plugin " + std::to_string(pluginId) + " configuration updated successfully"
                                            config,
                                            });
                                            } catch (error) {
                                                std::cerr << '[PLUGINS API] Error updating plugin config:' << error << std::endl;
                                                sendError(;
                                                res,
                                                500,
                                                'PLUGIN_CONFIG_ERROR',
                                                'Error updating plugin configuration',
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
