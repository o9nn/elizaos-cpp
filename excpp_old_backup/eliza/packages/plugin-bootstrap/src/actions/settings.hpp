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
 * Interface representing the structure of a setting update object.
 * @interface
 * @property {string} key - The key of the setting to be updated.
 * @property {string|boolean} value - The new value for the setting, can be a string or a boolean.
 */
/**
 * Interface for updating settings.
 * @typedef {Object} SettingUpdate
 * @property {string} key - The key of the setting to update.
 * @property {string | boolean} value - The new value of the setting, can be a string or a boolean.
 */
struct SettingUpdate {
    std::string key;
    string | boolean value;
};

// Template for success responses when settings are updated
/**
 * JSDoc comment for successTemplate constant
 *
 * # Task: Generate a response for successful setting updates
 * {{providers}}
 *
 * # Update Information:
 * - Updated Settings: {{updateMessages}}
 * - Next Required Setting: {{nextSetting.name}}
 * - Remaining Required Settings: {{remainingRequired}}
 *
 * # Instructions:
 * 1. Acknowledge the successful update of settings
 * 2. Maintain {{agentName}}'s personality and tone
 * 3. Provide clear guidance on the next setting that needs to be configured
 * 4. Explain what the next setting is for and how to set it
 * 5. If appropriate, mention how many required settings remain
 *
 * Write a natural, conversational response that {{agentName}} would send about the successful update and next steps.
 * Include the actions array ["SETTING_UPDATED"] in your response.
 * ${messageCompletionFooter}
 */

// Template for failure responses when settings couldn't be updated
/**
 * Template for generating a response for failed setting updates.
 *
 * @template T
 * @param {string} failureTemplate - The failure template string to fill in with dynamic content.
 * @returns {string} - The filled-in template for generating the response.
 */

// Template for error responses when unexpected errors occur
/**
 * Template for generating a response for an error during setting updates.
 *
 * The template includes placeholders for agent name, bio, recent messages,
 * and provides instructions for crafting a response.
 *
 * Instructions:
 * 1. Apologize for the technical difficulty
 * 2. Maintain agent's personality and tone
 * 3. Suggest trying again or contacting support if the issue persists
 * 4. Keep the message concise and helpful
 *
 * Actions array to include: ["SETTING_UPDATE_ERROR"]
 */

// Template for completion responses when all required settings are configured
/**
 * Task: Generate a response for settings completion
 *
 * About {{agentName}}:
 * {{bio}}
 *
 * Settings Status:
 * {{settingsStatus}}
 *
 * Recent Conversation:
 * {{recentMessages}}
 *
 * Instructions:
 * 1. Congratulate the user on completing the settings process
 * 2. Maintain {{agentName}}'s personality and tone
 * 3. Summarize the key settings that have been configured
 * 4. Explain what functionality is now available
 * 5. Provide guidance on what the user can do next
 * 6. Express enthusiasm about working together
 *
 * Write a natural, conversational response that {{agentName}} would send about the successful completion of settings.
 * Include the actions array ["ONBOARDING_COMPLETE"] in your response.
 */

/**
 * Gets settings state from world metadata
 */
/**
 * Retrieves the settings for a specific world from the database.
 * @param {IAgentRuntime} runtime - The Agent Runtime instance.
 * @param {string} serverId - The ID of the server.
 * @returns {Promise<WorldSettings | null>} The settings of the world, or null if not found.
 */
std::future<WorldSettings | null> getWorldSettings(IAgentRuntime runtime, const std::string& serverId);

/**
 * Updates settings state in world metadata
 */
std::future<bool> updateWorldSettings(IAgentRuntime runtime, const std::string& serverId, WorldSettings worldSettings);

/**
 * Formats a list of settings for display
 */
std::string formatSettingsList(WorldSettings worldSettings);

/**
 * Categorizes settings by their configuration status
 */
void categorizeSettings(WorldSettings worldSettings); {

    // Skip internal settings

/**
 * Extracts setting values from user message with improved handling of multiple settings
 */

/**
 * Processes multiple setting updates atomically
 */

    // Create a copy of the state for atomic updates

    // Process all updates

      // Check dependencies if they exist

      // Update the setting

      // Execute onSetAction if defined

    // If any updates were made, save the entire state to world metadata
      // Save to world metadata

      // Verify save by retrieving it again

/**
 * Handles the completion of settings when all required settings are configured
 */
std::future<void> handleOnboardingComplete(IAgentRuntime runtime, WorldSettings worldSettings, State _state, HandlerCallback callback);

/**
 * Generates a success response for setting updates
 */
std::future<void> generateSuccessResponse(IAgentRuntime runtime, WorldSettings worldSettings, State state, const std::vector<std::string>& messages, HandlerCallback callback);

/**
 * Generates a failure response when no settings could be updated
 */
std::future<void> generateFailureResponse(IAgentRuntime runtime, WorldSettings worldSettings, State state, HandlerCallback callback);

/**
 * Generates an error response for unexpected errors
 */
std::future<void> generateErrorResponse(IAgentRuntime runtime, State state, HandlerCallback callback);

/**
 * Enhanced settings action with improved state management and logging
 * Updated to use world metadata instead of cache
 */

      // Find the server where this user is the owner

      // Check if there's an active settings state in world metadata

      // Find the server where this user is the owner
        await generateErrorResponse(runtime, state, callback);

      // Get settings state from world metadata

        await generateErrorResponse(runtime, state, callback);

      // Extract setting values from message

      // Process extracted settings

      // Generate appropriate response

        // Get updated settings state
          await generateErrorResponse(runtime, state, callback);

        await generateFailureResponse(runtime, worldSettings, state, callback);
        await generateErrorResponse(runtime, state, callback);

} // namespace elizaos
