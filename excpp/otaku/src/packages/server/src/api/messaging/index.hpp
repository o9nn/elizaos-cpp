#include "channels.hpp"
#include "core.hpp"
#include "jobs.hpp"
#include "servers.hpp"
#include "sessions.hpp"
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

import type { ElizaOS } from '@elizaos/core';
;
import type { AgentServer } from '../../index';
;
;
;
;
;

/**
 * Creates the messaging router for all communication functionality
 */


  // Mount core messaging functionality at root level
  router.use('/', createMessagingCoreRouter(serverInstance));

  // Mount server management functionality
  router.use('/', createServersRouter(serverInstance));

  // Mount channel management functionality
  router.use('/', createChannelsRouter(elizaOS, serverInstance));

  // Mount unified sessions API for simplified messaging
  router.use('/', createSessionsRouter(elizaOS, serverInstance));

  // Mount jobs API for one-off messaging (similar to @bankr/sdk)
  router.use('/', createJobsRouter(elizaOS, serverInstance));

  return router;
}

} // namespace elizaos
