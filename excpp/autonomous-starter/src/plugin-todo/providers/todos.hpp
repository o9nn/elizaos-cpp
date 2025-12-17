#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".pointsService.hpp"
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * The TodosProvider fetches and formats information about a user's tasks and points.
 */

      // Fetch room details directly to get worldId

      // Get user's points (scoped to current interaction context)

      // Get ALL tasks for THIS ENTITY across all rooms/worlds

      // Filter out completed tasks from active

      // Get completed tasks in the last 7 days

        // Check completion date if available

        // If no completedAt, use updatedAt as fallback
            // If neither string nor Date-like, convert to string and try to parse

      // --- Format different types of tasks ---

      // Daily recurring tasks

      // One-off tasks with due dates

      // Aspirational goals (no due date)

      // Recently completed tasks

                // If neither string nor Date-like, convert to string and try to parse

      // Build the provider output

      // Points summary

      // Daily tasks

      // One-off tasks

      // Aspirational tasks

      // Recently completed tasks

      // Construct response object

      // Return a simple error message if something goes wrong


} // namespace elizaos
