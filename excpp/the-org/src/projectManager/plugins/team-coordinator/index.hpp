#include "actions/checkInCreate.hpp"
#include "actions/checkInFormatAction.hpp"
#include "actions/checkInList.hpp"
#include "actions/reportGenerate.hpp"
#include "actions/teamMemberAdd.hpp"
#include "actions/teamMemberUpdate.hpp"
#include "actions/teamMembersList.hpp"
#include "actions/updateFormat.hpp"
#include "elizaos/core.hpp"
#include "services/CheckInService.hpp"
#include "services/updateTracker.hpp"
#include "tasks.hpp"
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

// biome-ignore lint/style/useImportType: <explanation>
import type { IAgentRuntime, Plugin } from '@elizaos/core';
// ;
;
;
;
;
;
;
;
;
;
;
;

/**
 * Plugin for team coordination functionality
 * Handles team member management, availability tracking, and check-ins
 */
const teamCoordinatorPlugin: Plugin = {
  name: 'team-coordinator',
  description: 'Team Coordinator plugin for managing team activities',
  providers: [],
  actions: [
    // checkInFormatAction,
    teamMemberUpdatesAction,
    listCheckInSchedules,
    generateReport,
    recordCheckInAction,
    addTeamMemberAction,
    listTeamMembersAction,
    updatesFormatAction,
  ],
  init: async (config: Record<string, string>, runtime: IAgentRuntime) => {
    try {
      logger.info('Initializing Team Coordinator plugin...');

      // Register the services
      logger.info('Registering TeamUpdateTrackerService...');
      await runtime.registerService(TeamUpdateTrackerService);

      // Register and start the CheckIn service
      // logger.info('Registering CheckInService...');
      // await runtime.registerService(CheckInService);

      // Delay task registration to ensure adapter is ready
      logger.info('Scheduling team coordinator tasks registration...');
      
      // Use a retry mechanism to register tasks when adapter is ready
      const registerTasksWithRetry = async (retries = 10, delay = 1000) => {
        for (let i = 0; i < retries; i++) {
          try {
            // Check if getTasks method is available
            if (runtime.getTasks && typeof runtime.getTasks === 'function') {
              logger.info('Runtime is ready, registering team coordinator tasks...');
              await registerTasks(runtime);
              logger.info('Team coordinator tasks registered successfully');
              return;
            } else {
              logger.info(`Runtime not ready yet, retrying in ${delay}ms... (attempt ${i + 1}/${retries})`);
            }
          } catch (error) {
            logger.warn(`Failed to register tasks (attempt ${i + 1}/${retries}):`, error);
          }
          
          // Wait before next retry
          await new Promise(resolve => setTimeout(resolve, delay));
        }
        
        logger.error('Failed to register team coordinator tasks after all retries');
      };

      // Start the retry process asynchronously
      registerTasksWithRetry().catch(error => {
        logger.error('Error in registerTasksWithRetry:', error);
      });

      logger.info('Team Coordinator plugin initialized successfully');
    } catch (error) {
      logger.error('Failed to initialize Team Coordinator plugin:', error);
      throw error;
    }
  },
  // List services that should be registered by the runtime
  services: [TeamUpdateTrackerService, CheckInService],
};

;
}

default {
  initialize,
};

} // namespace elizaos
