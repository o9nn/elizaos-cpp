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
import type { AgentServer } from '../../index';
;

/**
 * Agent lifecycle operations (start, stop, status)
 */

    if (!db) {
      return sendError(res, 500, 'DB_ERROR', 'Database not available');
    }

    try {
      const agent = await db.getAgent(agentId);

      if (!agent) {
        logger.debug('[AGENT START] Agent not found');
        return sendError(res, 404, 'NOT_FOUND', 'Agent not found');
      }

      const isActive = !!agents.get(agentId);

      if (isActive) {
        logger.debug(`[AGENT START] Agent ${agentId} is already running`);
        return sendSuccess(res, {
          id: agentId,
          name: agent.name,
          status: 'active',
        });
      }

      await serverInstance?.startAgent(agent);

      const runtime = agents.get(agentId);
      if (!runtime) {
        throw new Error('Failed to start agent');
      }

      logger.debug(`[AGENT START] Successfully started agent: ${agent.name}`);
      sendSuccess(res, {
        id: agentId,
        name: agent.name,
        status: 'active',
      });
    } catch (error) {
      logger.error('[AGENT START] Error starting agent:', error);
      sendError(
        res,
        500,
        'START_ERROR',
        'Error starting agent',
        error instanceof Error ? error.message : String(error)
      );
    }
  });

  // Stop an existing agent
  router.post('/:agentId/stop', async (req, res) => {
    const agentId = validateUuid(req.params.agentId);
    if (!agentId) {
      logger.debug('[AGENT STOP] Invalid agent ID format');
      return sendError(res, 400, 'INVALID_ID', 'Invalid agent ID format');
    }

    const runtime = agents.get(agentId);
    if (!runtime) {
      return sendError(res, 404, 'NOT_FOUND', 'Agent not found');
    }

    serverInstance?.unregisterAgent(agentId);

    logger.debug(`[AGENT STOP] Successfully stopped agent: ${runtime.character.name} (${agentId})`);

    sendSuccess(res, {
      message: 'Agent stopped',
    });
  });

  return router;
}

} // namespace elizaos
