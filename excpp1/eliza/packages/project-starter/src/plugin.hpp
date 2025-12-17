#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Define the configuration schema for the plugin with the following properties:
 *
 * @param {string} EXAMPLE_PLUGIN_VARIABLE - The name of the plugin (min length of 1, optional)
 * @returns {object} - The configured schema object
 */

/**
 * Example HelloWorld action
 * This demonstrates the simplest possible action structure
 */
/**
 * Represents an action that responds with a simple hello world message.
 *
 * @typedef {Object} Action
 * @property {string} name - The name of the action
 * @property {string[]} similes - The related similes of the action
 * @property {string} description - Description of the action
 * @property {Function} validate - Validation function for the action
 * @property {Function} handler - The function that handles the action
 * @property {Object[]} examples - Array of examples for the action
 */

    // Always valid

      // Simple response content

      // Call back with the hello world message

/**
 * Example Hello World Provider
 * This demonstrates the simplest possible provider implementation
 */

class StarterService {
public:
    StarterService(IAgentRuntime runtime);
    void start(IAgentRuntime runtime);
    void stop(IAgentRuntime runtime);
    void stop();
};

  // Set lowest priority so real models take precedence

      // Set all environment variables at once
        // send a response
        // print the keys
        // print the keys
        // print the keys
        // print the keys


} // namespace elizaos
