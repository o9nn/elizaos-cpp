#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "elizaos/core.hpp"
#include "tests.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Defines the configuration schema for a plugin, including the validation rules for the plugin name.
 *
 * @type {import('zod').ZodObject<{ EXAMPLE_PLUGIN_VARIABLE: import('zod').ZodString }>}
 */

/**
 * Example HelloWorld action
 * This demonstrates the simplest possible action structure
 */
/**
 * Action representing a hello world message.
 * @typedef {Object} Action
 * @property {string} name - The name of the action.
 * @property {string[]} similes - An array of related actions.
 * @property {string} description - A brief description of the action.
 * @property {Function} validate - Asynchronous function to validate the action.
 * @property {Function} handler - Asynchronous function to handle the action and generate a response.
 * @property {Object[]} examples - An array of example inputs and expected outputs for the action.
 */

    // Always valid

      // Simple response content

      // Call back with the hello world message if callback is provided

/**
 * Example Hello World Provider
 * This demonstrates the simplest possible provider implementation
 */

class StarterService {
public:
    StarterService(IAgentRuntime protected runtime);
    void start(IAgentRuntime runtime);
    void stop(IAgentRuntime runtime);
    void stop();
};

      // Set all environment variables at once
        // send a response
        // Return current time in various formats
        // print the keys
        // print the keys
        // print the keys
        // print the keys
  // dependencies: ['@elizaos/plugin-knowledge'], <--- plugin dependecies go here (if requires another plugin)


} // namespace elizaos
