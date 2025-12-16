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

/**
 * Health monitoring and status endpoints
 */
);
  });

  // Hello world endpoint
  router.get('/hello', (_req, res) => {
    logger.info('Hello endpoint hit');
    res.setHeader('Content-Type', 'application/json');
    res.send(JSON.stringify({ message: 'Hello World!' }));
  });

  // System status endpoint
  router.get('/status', (_req, res) => {
    logger.info('Status endpoint hit');
    res.setHeader('Content-Type', 'application/json');
    res.send(
      JSON.stringify({
        status: 'ok',
        agentCount: agents.size,
        timestamp: new Date().toISOString(),
      })
    );
  });

  // Comprehensive health check
  router.get('/health', (_req, res) => {
    logger.log({ apiRoute: '/health' }, 'Health check route hit');
    const healthcheck = {
      status: 'OK',
      version: process.env.APP_VERSION || 'unknown',
      timestamp: new Date().toISOString(),
      dependencies: {
        // Server is healthy even with no agents - it's ready to accept agent registrations
        agents: agents.size > 0 ? 'healthy' : 'ready',
      },
    };

    // Always return 200 if server is running - agent count doesn't affect server health
    res.status(200).json(healthcheck);
  });

  // Server stop endpoint
  router.post('/stop', (_req, res) => {
    logger.log({ apiRoute: '/stop' }, 'Server stopping...');
    serverInstance?.stop(); // Use optional chaining in case server is undefined
    res.json({ message: 'Server stopping...' });
  });

  return router;
}

} // namespace elizaos
