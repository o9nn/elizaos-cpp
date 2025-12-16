#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

;
;
;

// Registry configuration - centralized for maintainability
const REGISTRY_ORG = 'elizaos-plugins';
const REGISTRY_REPO = 'registry';
const REGISTRY_URL = `https://raw.githubusercontent.com/${REGISTRY_ORG}/${REGISTRY_REPO}/refs/heads/main/generated-registry.json`;

struct GitVersionInfo {
    string | null version;
    string | null branch;
};


struct PluginGitInfo {
    std::string repo;
    GitVersionInfo v0;
    GitVersionInfo v1;
};


struct PluginNpmInfo {
    std::string repo;
    string | null v0;
    string | null v1;
};


struct PluginSupport {
    bool v0;
    bool v1;
};


struct PluginInfo {
    PluginGitInfo git;
    PluginNpmInfo npm;
    PluginSupport supports;
};


struct RegistryResponse {
    std::string lastUpdatedAt;
    std::unordered_map<std::string, PluginInfo> registry;
};


/**
 * Function to fetch plugins data from the registry API and merge with agent plugins.
 * @returns {UseQueryResult<PluginEntry[]>} Query result containing array of plugin entries
 */
)
          .filter(([name, data]: [string, PluginInfo]) => {
            // Check if it's a plugin and has v1 support
            const isPlugin = name.includes('plugin');
            const hasV1Support = data.supports.v1 === true;
            const hasV1Version =
              data.npm.v1 !== null || (data.git.v1.version !== null && data.git.v1.branch !== null);

            return isPlugin && hasV1Support && hasV1Version;
          })
          .map(([name]) => name.replace(/^@elizaos-plugins\//, '@elizaos/'))
          .sort();

        // Process agent plugins from the parallel fetch
        let agentPlugins: string[] = [];
        try {
          if (agentsResponse?.length > 0) {
            // Get plugins from the first active agent
            const activeAgent = agentsResponse.find((agent) => agent.status === 'active');
            if (activeAgent && activeAgent.id) {
              const agentDetailResponse = await elizaClient.agents.getAgent(activeAgent.id);

              if (agentDetailResponse?.plugins) {
                agentPlugins = agentDetailResponse.plugins;
              }
            }
          }
        } catch (agentError) {
          clientLogger.warn('Could not fetch agent plugins:', agentError);
        }

        // Merge registry plugins with agent plugins and remove duplicates
        const allPlugins = [...new Set([...registryPlugins, ...agentPlugins])];
        return allPlugins.sort();
      } catch (error) {
        clientLogger.error('Failed to fetch from registry, falling back to basic list:', error);

        // Return fallback plugins with basic info
        return [
          '@elizaos/plugin-bootstrap',
          '@elizaos/plugin-evm',
          '@elizaos/plugin-discord',
          '@elizaos/plugin-elevenlabs',
          '@elizaos/plugin-anthropic',
          '@elizaos/plugin-browser',
          '@elizaos/plugin-farcaster',
          '@elizaos/plugin-groq',
        ]
          .filter((name) => name.includes('plugin'))
          .sort();
      }
    },
  });
}

} // namespace elizaos
