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
;

/**
 * Agent panels and plugin routes management
 */


    const runtime = agents.get(agentId);
    if (!runtime) {
      return sendError(res, 404, 'NOT_FOUND', 'Agent not found');
    }

    try {
      const publicPanels = runtime.plugins
        .flatMap((plugin) => plugin.routes || [])
        .filter((route) => route.public === true && route.type === 'GET' && route.name)
        .map((route) => ({
          name: route.name,
          path: `/api${route.path.startsWith('/') ? route.path : `/${route.path}`}?agentId=${agentId}`,
        }));

      sendSuccess(res, publicPanels);
    } catch (error) {
      logger.error(`[AGENT PANELS] Error retrieving panels for agent ${agentId}:`, error);
      sendError(
        res,
        500,
        'PANEL_ERROR',
        'Error retrieving agent panels',
        error instanceof Error ? error.message : String(error)
      );
    }
  });

  return router;
}

} // namespace elizaos
