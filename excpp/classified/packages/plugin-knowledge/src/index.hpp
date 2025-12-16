#include "actions.hpp"
#include "elizaos/core.hpp"
#include "provider.hpp"
#include "routes.hpp"
#include "service.hpp"
#include "tests.hpp"
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

/**
 * Knowledge Plugin - Main Entry Point
 *
 * This file exports all the necessary functions and types for the Knowledge plugin.
 */
import type { Plugin, IAgentRuntime } from '@elizaos/core';
;
;
;
;
;
;

/**
 * Knowledge Plugin - Provides Retrieval Augmented Generation capabilities
 */
const knowledgePlugin: Plugin = {
  name: 'knowledge',
  description:
    'Plugin for Retrieval Augmented Generation, including knowledge management and embedding.',
  services: [KnowledgeService],
  providers: [knowledgeProvider],
  routes: knowledgeRoutes,
  actions: knowledgeActions,
  tests: [knowledgeTestSuite],
};

default knowledgePlugin;

// Export service for external use
{ KnowledgeService } from './service';

* from './types';

} // namespace elizaos
