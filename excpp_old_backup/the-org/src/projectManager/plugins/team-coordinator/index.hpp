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
// 

/**
 * Plugin for team coordination functionality
 * Handles team member management, availability tracking, and check-ins
 */
    // checkInFormatAction,

      // Register the services

      // Register and start the CheckIn service
      // logger.info('Registering CheckInService...');
      // await runtime.registerService(CheckInService);

      // Delay task registration to ensure adapter is ready
      
      // Use a retry mechanism to register tasks when adapter is ready
            // Check if getTasks method is available
              await registerTasks(runtime);
          
          // Wait before next retry

      // Start the retry process asynchronously

  // List services that should be registered by the runtime

void initialize(IAgentRuntime runtime);


} // namespace elizaos
