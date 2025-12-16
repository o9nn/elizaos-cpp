#include ".shared/response-utils.hpp"
#include "elizaos/core.hpp"
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

import type { IAgentRuntime, UUID } from '@elizaos/core';
;
;
import type { AgentServer } from '../../server';
;

struct PluginConfig {
    std::string id;
    std::string name;
    std::string displayName;
    bool enabled;
    std::string version;
    std::string description;
    std::unordered_map<std::string, std::any> config;
};


/**
 * Agent plugin configuration management
 */


      // Return plugin configurations based on agent's loaded plugins
      const pluginConfigs: PluginConfig[] = [
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
            model: runtime.character.settings?.model || 'gpt-4o-mini',
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
      logger.error('[PLUGINS API] Error getting plugin configs:', error);
      sendError(
        res,
        500,
        'PLUGINS_ERROR',
        'Error retrieving plugin configurations',
        error instanceof Error ? error.message : String(error)
      );
    }
  });

  // Update plugin configuration
  router.post('/:agentId/plugins/:pluginId/config', async (req, res) => {
    try {
      const agentId = validateUuid(req.params.agentId);
      const pluginId = req.params.pluginId;
      const { config } = req.body;

      const runtime = agents.get(agentId);
      if (!runtime) {
        return sendError(res, 404, 'NOT_FOUND', 'Agent not found or not running');
      }

      logger.info(
        `[PLUGINS API] Updated config for plugin ${pluginId} on agent ${runtime.character.name}`
      );
      sendSuccess(res, {
        message: `Plugin ${pluginId} configuration updated successfully`,
        config,
      });
    } catch (error) {
      logger.error('[PLUGINS API] Error updating plugin config:', error);
      sendError(
        res,
        500,
        'PLUGIN_CONFIG_ERROR',
        'Error updating plugin configuration',
        error instanceof Error ? error.message : String(error)
      );
    }
  });

  return router;
}

} // namespace elizaos
