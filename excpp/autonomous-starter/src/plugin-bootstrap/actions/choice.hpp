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



/**
 * Task: Extract selected task and option from user message
 *
 * Available Tasks:
 * {{#each tasks}}
 * Task ID: {{taskId}} - {{name}}
 * Available options:
 * {{#each options}}
 * - {{name}}: {{description}}
 * {{/each}}
 * - ABORT: Cancel this task
 * {{/each}}
 *
 * Recent Messages:
 * {{recentMessages}}
 *
 * Instructions:
 * 1. Review the user's message and identify which task and option they are selecting
 * 2. Match against the available tasks and their options, including ABORT
 * 3. Return the task ID (shortened UUID) and selected option name exactly as listed above
 * 4. If no clear selection is made, return null for both fields
 *
 * Return in JSON format:
 * ```json
 * {
 *   "taskId": "string" | null,
 *   "selectedOption": "OPTION_NAME" | null
 * }
 * ```
 *
 * Make sure to include the ```json``` tags around the JSON object.
 */

/**
 * Represents an action that allows selecting an option for a pending task that has multiple options.
 * @type {Action}
 * @property {string} name - The name of the action
 * @property {string[]} similes - Similar words or phrases for the action
 * @property {string} description - A brief description of the action
 * @property {Function} validate - Asynchronous function to validate the action
 * @property {Function} handler - Asynchronous function to handle the action
 * @property {ActionExample[][]} examples - Examples demonstrating the usage of the action
 */

    // Get all tasks tagged AWAITING_CHOICE

    // Filter for tasks that actually have options defined in metadata

    // If no tasks are actually awaiting a choice with options, fail validation

    // --- Role Check Modification ---
    // Check if ANY of the pending tasks are TODO confirmations

    // If it IS a TODO confirmation, allow any user (skip role check)
    // --- End Role Check Modification ---

    // If we passed the checks (task exists, role is ok or skipped), validation passes

        // This case should ideally be caught by validate, but double-check

      // --- Smarter Extraction Logic ---
        // --- Case 1: Only one pending task ---

          // We already know the targetTaskFullId
        // --- Case 2: Multiple pending tasks (Original Logic) ---
              // Add try-catch for safety

          // Find the task by matching the shortened UUID
      // --- End Smarter Extraction Logic ---

      // --- Proceed if we have a selection and a target task ---

        // Find the actual task object using the full ID

          // Should not happen if targetTaskFullId is set, but check anyway

        // --- Update Task for Specific Handler Worker ---

        // Determine the target handler worker name based on the original action
          // Add cases for other actions that use this confirmation flow
            // Assuming the twitter post task is named 'Confirm Twitter Post'
            // We might need a different approach if it registers its own worker directly.
            // For now, let's assume it needs a handler worker if it followed this pattern.

        // Update the task: change name, remove AWAITING_CHOICE, add selected option to metadata

          // Optionally provide immediate feedback, or let the handler worker provide it.
          // A simple acknowledgement might be good here.
        // Fallback: If no valid selection could be determined after trying single/multi prompts


} // namespace elizaos
