#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "elizaos/core.hpp"
#include "tasks/chat.hpp"
#include "tasks/twitter.hpp"
#include "tasks/twitterParser.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// let's not make it a dependency

/**
 * Registers tasks for the agent to perform various Intel-related actions.
 * * @param { IAgentRuntime } runtime - The agent runtime object.
 * @param { UUID } [worldId] - The optional world ID to associate with the tasks.
 * @returns {Promise<void>} - A promise that resolves once tasks are registered.
 */

  // first, get all tasks with tags "queue", "repeat", "autofun" and delete them

  // shouldn't plugin-solana and plugin-evm handle this?
      /*
        // kill this task
      */

        // kill this task

  // Only create the Twitter sync task if the Twitter service exists
        // Check if Twitter service exists and return false if it doesn't
          // Log only once when we'll be removing the task

          // Get all tasks of this type

          // Delete all these tasks

        // Check if Twitter service exists and return false if it doesn't
          // The main task handler above will take care of removing all Twitter tasks


} // namespace elizaos
