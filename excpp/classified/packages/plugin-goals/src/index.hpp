#include "__tests__/e2e.hpp"
#include "actions/cancelGoal.hpp"
#include "actions/completeGoal.hpp"
#include "actions/confirmGoal.hpp"
#include "actions/createGoal.hpp"
#include "actions/updateGoal.hpp"
#include "apis.hpp"
#include "elizaos/core.hpp"
#include "providers/goals.hpp"
#include "schema.hpp"
#include "services/goalService.hpp"
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

import type { Plugin } from '@elizaos/core';
;

;

// Import actions
;
;
;
;
;

// Import providers
;

// Import services
;

// Import schema
;

// Note: Table schemas are defined in schema.ts and will be automatically migrated

// Import tests
;

/**
 * The GoalsPlugin provides goal management functionality,
 * including creating, completing, updating, and canceling goals.
 */
const GoalsPlugin: Plugin = {
  name: 'goals',
  description: 'Provides goal management functionality for tracking and achieving objectives.',
  providers: [goalsProvider],
  dependencies: ['@elizaos/plugin-sql'],
  testDependencies: ['@elizaos/plugin-sql'],
  actions: [
    createGoalAction,
    completeGoalAction,
    confirmGoalAction,
    updateGoalAction,
    cancelGoalAction,
  ],
  schema: goalSchemaExport,
  services: [GoalService],
  routes,
  tests: e2eTestSuites,

  async init(config: Record<string, string>, runtime: IAgentRuntime): Promise<void> {
    try {
      logger.info('[Goals Plugin] Initializing...');

      // Log current services to see what's registered
      const services = (runtime as any).services || new Map();
      logger.info('[Goals Plugin] Services at init start:', Array.from(services.keys()));

      // Goals tables will be created through the standard schema system
      // The tables are defined in schema.ts and will be migrated automatically
      // when the plugin schema is processed by the SQL plugin

      // Database migrations are handled by the SQL plugin
      if (runtime.db) {
        logger.info('[Goals Plugin] Database available, GoalsPlugin ready for operation');

        // Note: Initial goals creation will be deferred until after migrations
        // This is handled by a separate initialization service
      } else {
        logger.warn('[Goals Plugin] No database instance available, operations will be limited');
      }

      logger.info('[Goals Plugin] GoalsPlugin initialized successfully');

      // Log services again to see if anything changed
      logger.info('[Goals Plugin] Services at init end:', Array.from(services.keys()));
    } catch (error) {
      logger.error('[Goals Plugin] Error initializing GoalsPlugin:', error);
      throw error;
    }
  },
};

default GoalsPlugin;

// Export service types for use in other plugins/tests
{ GoalService } from './services/goalService';
{ type GoalData } from './types';

// Helper 

} // namespace elizaos
