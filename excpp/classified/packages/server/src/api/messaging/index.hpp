#include "channels.hpp"
#include "core.hpp"
#include "servers.hpp"
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
import type { AgentServer } from '../../index';
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
  router.use('/', createChannelsRouter(agents, serverInstance));

  return router;
}

} // namespace elizaos
