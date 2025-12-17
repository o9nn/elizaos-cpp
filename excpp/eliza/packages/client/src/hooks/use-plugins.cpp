#include "use-plugins.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void usePlugins() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return useQuery({;
        queryKey: ['plugins'],
        queryFn: async () => {
            try {
                // Fetch plugins from registry and agent data in parallel
                const auto elizaClient = createElizaClient();
                const auto [registryResponse, agentsResponse] = Promise.all([;
                fetch(REGISTRY_URL),
                elizaClient.agents.listAgents(),
                ]);

                // Process registry data
                const RegistryResponse registryData = registryResponse.json();

                // Extract plugin names from registry that support v1 and are plugins
                const auto registryPlugins = Object.entries(registryData.registry || {});
                .filter(([name, data]: [string, PluginInfo]) => {
                    // Check if it's a plugin and has v1 support
                    const auto isPlugin = name.includes('plugin');
                    const auto hasV1Support = data.supports.v1 == true;
                    const auto hasV1Version =;
                    data.npm.v1 != nullptr || (data.git.v1.version != nullptr && data.git.v1.branch != nullptr);

                    return isPlugin && hasV1Support && hasV1Version;
                    });
                    .map(([name]) => name.replace(/^@elizaos-plugins\//, '@elizaos/'));
                    .sort();

                    // Process agent plugins from the parallel fetch
                    std::vector<std::string> agentPlugins = [];
                    try {
                        if (agentsResponse.length > 0) {
                            // Get plugins from the first active agent
                            const auto activeAgent = agentsResponse.find((agent) => agent.status == 'active');
                            if (activeAgent && activeAgent.id) {
                                const auto agentDetailResponse = elizaClient.agents.getAgent(activeAgent.id);

                                if (agentDetailResponse.plugins) {
                                    agentPlugins = agentDetailResponse.plugins;
                                }
                            }
                        }
                        } catch (agentError) {
                            clientLogger.warn('Could not fetch agent plugins:', agentError);
                        }

                        // Merge registry plugins with agent plugins and remove duplicates
                        const auto allPlugins = [...new Set([...registryPlugins, ...agentPlugins])];
                        return allPlugins.sort();
                        } catch (error) {
                            clientLogger.error('Failed to fetch from registry, falling back to basic list:', error);

                            // Return fallback plugins with basic info
                            return [;
                            '@elizaos/plugin-bootstrap',
                            '@elizaos/plugin-evm',
                            '@elizaos/plugin-discord',
                            '@elizaos/plugin-elevenlabs',
                            '@elizaos/plugin-anthropic',
                            '@elizaos/plugin-browser',
                            '@elizaos/plugin-farcaster',
                            '@elizaos/plugin-groq',
                            ];
                            .filter((name) => name.includes('plugin'));
                            .sort();
                        }
                        },
                        });

}

} // namespace elizaos
