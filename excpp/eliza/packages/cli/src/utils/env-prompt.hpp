#pragma once
#include <functional>
#include <future>
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
 * Interface for environment variable configuration
 */
struct EnvVarConfig {
    std::string name;
    std::string key;
    bool required;
    std::string description;
    std::string url;
    bool secret;
};

// Configuration for known environment variables by plugin name

/**
 * Retrieves the absolute path to the `.env` environment file.
 *
 * @returns A promise that resolves to the full path of the environment file.
 */
std::future<std::string> getEnvFilePath();

/**
 * Asynchronously reads environment variables from the `.env` file and returns them as key-value pairs.
 *
 * Ignores comments and empty lines. If the file does not exist or cannot be read, returns an empty object.
 *
 * @returns A record containing environment variable names and their corresponding values.
 */

/**
 * Asynchronously writes the provided environment variables to the `.env` file, creating the directory if it does not exist.
 *
 * @param envVars - A record of environment variable key-value pairs to write.
 */
std::future<void> writeEnvFile(const std::unordered_map<std::string, std::string>& envVars);

/**
 * Prompts the user to enter a value for a specific environment variable based on the provided configuration.
 *
 * Displays the variable's description and an optional URL for guidance. Uses masked input for secrets.
 * For optional variables, allows skipping by pressing Enter. For the `PGLITE_DATA_DIR` variable, expands a leading tilde to the project directory.
 *
 * @param config - The configuration describing the environment variable to prompt for.
 * @returns The entered or existing value, or an empty string if an optional variable is skipped.
 */
std::future<std::string> promptForEnvVar(EnvVarConfig config);

/**
 * Prompts the user to enter missing or invalid environment variables required for a specified plugin.
 *
 *
 * @param pluginName - The name of the plugin to configure (e.g., 'openai', 'discord').
 * @returns A record containing the environment variables that were set during the prompt.
 */

/**
 * Determines whether all required environment variables for a given plugin are set and valid.
 *
 * @param pluginName - The name of the plugin to validate environment variables for.
 * @returns `true` if all required environment variables are present and not set to placeholder values; otherwise, `false`.
 */
std::future<bool> validateEnvVars(const std::string& pluginName);

/**
 * Returns the keys of required environment variables that are missing or set to placeholder values for the specified plugin.
 *
 * @param pluginName - The name of the plugin to check for missing environment variables.
 * @returns An array of keys for required environment variables that are not set or have placeholder values.
 */

/**
 * Validates environment variable configuration for a specific plugin and provides a status message.
 *
 * Checks whether all required environment variables for the given {@link pluginName} are set and valid, applying plugin-specific rules for Discord, Twitter, Telegram, OpenAI, Anthropic, and PostgreSQL. Returns an object indicating whether the configuration is valid and a descriptive message.
 *
 * @param pluginName - The name of the plugin to validate.
 * @returns An object containing a boolean `valid` flag and a `message` describing the validation result.
 */

      // Check if Discord token is provided but application ID is not

      // Check if both are provided and valid

      // If neither is provided, that's fine - Discord is optional

      // Check if all Twitter credentials are provided

      // Check if any Twitter credentials are provided but not all

      // If all are provided, Twitter is properly configured

      // If none are provided, that's fine - Twitter is optional


} // namespace elizaos
