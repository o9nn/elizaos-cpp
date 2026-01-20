#include "elizaos/core.hpp"
#include "utils.hpp"
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
 * Validation service for Anthropic plugin
 * Provides runtime validation for API key availability
 */
class AnthropicValidationService extends Service {
  static override serviceType: ServiceTypeName = 'anthropic-validation' as ServiceTypeName;
  static serviceName = 'anthropic-validation';

  override capabilityDescription = 'Validates Anthropic plugin configuration and API availability';

  constructor(runtime: IAgentRuntime) {
    super(runtime);
  }

    // No cleanup needed

  /**
   * Check if Anthropic is properly configured and available
   */

/**
 * Retrieves a configuration setting from the runtime, falling back to environment variables or a default value if not found.
 *
 * @param key - The name of the setting to retrieve.
 * @param defaultValue - The value to return if the setting is not found in the runtime or environment.
 * @returns The resolved setting value, or {@link defaultValue} if not found.
 */

/**
 * Helper function to get the API key for Anthropic
 *
 * @param runtime The runtime context
 * @returns The configured API key
 */

/**
 * Helper function to get the experimental telemetry setting
 *
 * @param runtime The runtime context
 * @returns Whether experimental telemetry is enabled
 */
bool getExperimentalTelemetry(IAgentRuntime runtime);

/**
 * Ensures that the Anthropic API key is available in the runtime or environment.
 * Throws an error if the API key is missing.
 *
 * @param runtime - The agent runtime environment to check for settings.
 */
void ensureAnthropicAPIKeyExists(IAgentRuntime runtime);

/**
 * Plugin for Anthropic.
 *
 * @type {Plugin}
 * @property {string} name - The name of the plugin.
 * @property {string} description - The description of the plugin.
 * @property {Object} config - The configuration object with API keys and model variables.
 * @property {Function} init - Initializes the plugin with the given configuration.
 * @property {Function} models - Contains functions for generating text using small and large models.
 * @property {Function[]} tests - An array of test functions for the plugin.
 */
      // Register the validation service

      // If API key is not set, we'll show a warning but continue
        // Return early without throwing an error
      // Convert to warning instead of error

        // Pass along any system prompt if available.

    // TEXT_LARGE generation using Anthropics (e.g. using a "claude-3" model).

        // Check if this is a reflection schema request (has specific format)

        // Build a prompt that asks for JSON output

        // Don't modify the prompt if it already contains explicit JSON formatting instructions

        // For reflection schemas, we need a more specific instruction

        // Generate text response that should contain JSON

        // Extract and parse JSON from the response with our improved function

          // Ensure reflection schema has all required properties

        // Check if this is a reflection schema request (has specific format)

        // Build a prompt that asks for JSON output

        // Don't modify the prompt if it already contains explicit JSON formatting instructions

        // For reflection schemas, we need a more specific instruction

        // Generate text response that should contain JSON

        // Extract and parse JSON from the response with our improved function

          // Ensure reflection schema has all required properties


} // namespace elizaos
