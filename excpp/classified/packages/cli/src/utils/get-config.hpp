#pragma once
#include <algorithm>
#include <any>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "elizaos/core.hpp"
#include "resolve-utils.hpp"
#include "spinner-utils.hpp"
#include "user-environment.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Database config schemas

/**
 * Default sample environment variables written to new .env files.
 */

/**
 * Validates a Postgres URL format
 * @param url The URL to validate
 * @returns True if the URL appears valid
 */
bool isValidPostgresUrl(const std::string& url);

/**
 * Retrieves the standard directory paths used by Eliza for configuration and database storage.
 *
 * @returns An object containing the Eliza configuration directory, the Eliza database directory for the current project, and the path to the Eliza `.env` file.
 */
std::future<void> getElizaDirectories(std::optional<std::string> targetProjectDir);

/**
 * Generic function to ensure a directory exists
 * @param dirPath Path to the directory
 */
std::future<void> ensureDir(const std::string& dirPath);

/**
 * Sets up the .env file by creating it if it doesn't exist or populating it with a hybrid
 * @param envFilePath Path to the .env file
 */
std::future<void> setupEnvFile(const std::string& envFilePath);

/**
 * Ensures the Eliza configuration directory exists and returns standard Eliza directory paths.
 *
 * @returns An object containing paths for the Eliza configuration directory, the Eliza database directory, and the `.env` file.
 */
std::future<void> ensureElizaDir(std::optional<std::string> targetProjectDir);

/**
 * Sets up and configures PGLite database
 * @param elizaDbDir The directory for PGLite database
 * @param envFilePath Path to the .env file
 */
std::future<void> setupPgLite(const std::string& dbDir, const std::string& envPath, std::optional<std::string> targetProjectDir);

/**
 * Stores the provided Postgres connection URL in the specified `.env` file, replacing any existing entry.
 *
 *
 * @param url - The Postgres connection URL to store.
 * @param envFilePath - Path to the `.env` file where the URL should be saved.
 *
 * @throws {Error} If reading from or writing to the `.env` file fails.
 */
std::future<void> storePostgresUrl(const std::string& url, const std::string& envFilePath);

/**
 * Stores the provided PGLite data directory in the specified `.env` file, replacing any existing entry.
 *
 *
 * @param dataDir - The PGLite data directory path to store.
 * @param envFilePath - Path to the `.env` file where the directory should be saved.
 *
 * @throws {Error} If reading from or writing to the `.env` file fails.
 */
std::future<void> storePgliteDataDir(const std::string& dataDir, const std::string& envFilePath);

/**
 * Prompts the user for a Postgres URL, validates it, and stores it
 * @returns The configured Postgres URL or null if user cancels
 */
std::future<std::string> promptAndStorePostgresUrl(const std::string& envFilePath);

/**
 * Validates an OpenAI API key format
 * @param key The API key to validate
 * @returns True if the key appears valid
 */
bool isValidOpenAIKey(const std::string& key);

/**
 * Validates an Anthropic API key format
 * @param key The API key to validate
 * @returns True if the key appears valid
 */
bool isValidAnthropicKey(const std::string& key);

/**
 * Validates a Google Generative AI API key format
 * @param key The API key to validate
 * @returns True if the key appears valid
 */
bool isValidGoogleKey(const std::string& key);

/**
 * Stores OpenAI API key in the .env file
 * @param key The OpenAI API key to store
 * @param envFilePath Path to the .env file
 */
std::future<void> storeOpenAIKey(const std::string& key, const std::string& envFilePath);

/**
 * Stores Google Generative AI API key in the .env file
 * @param key The Google API key to store
 * @param envFilePath Path to the .env file
 */
std::future<void> storeGoogleKey(const std::string& key, const std::string& envFilePath);

/**
 * Stores Anthropic API key in the .env file
 * @param key The Anthropic API key to store
 * @param envFilePath Path to the .env file
 */
std::future<void> storeAnthropicKey(const std::string& key, const std::string& envFilePath);

/**
 * Generic configuration for provider prompts
 */
struct ProviderPromptConfig {
    std::string name;
    std::string icon;
    std::string noteText;
    std::string key;
    std::string message;
    std::optional<std::string> placeholder;
    std::optional<std::string> initialValue;
    std::string type;
    std::string successMessage;
};

/**
 * Generic function to prompt for provider configuration
 * @param config Provider-specific configuration
 * @param envFilePath Path to the .env file
 * @returns The configured values or null if user cancels
 */

  // Collect all inputs

  // Store the configuration

/**
 * Prompts the user for an OpenAI API key, validates it, and stores it
 * @param envFilePath Path to the .env file
 * @returns The configured OpenAI API key or null if user cancels
 */
std::future<std::string> promptAndStoreOpenAIKey(const std::string& envFilePath);

/**
 * Prompts the user for an Anthropic API key, validates it, and stores it
 * @param envFilePath Path to the .env file
 * @returns The configured Anthropic API key or null if user cancels
 */
std::future<std::string> promptAndStoreAnthropicKey(const std::string& envFilePath);

/**
 * Validates an Ollama API endpoint format
 * @param endpoint The endpoint URL to validate
 * @returns True if the endpoint appears valid
 */
bool isValidOllamaEndpoint(const std::string& endpoint);

/**
 * Stores Ollama configuration in the .env file
 * @param config The Ollama configuration to store
 * @param envFilePath Path to the .env file
 */
std::future<void> storeOllamaConfig(const std::any& config, const std::string& envFilePath);

/**
 * Prompts the user for Ollama embedding model selection
 * @param envFilePath Path to the .env file
 * @returns The configured Ollama embedding settings or null if user cancels
 */
  // Check if we already have an Ollama endpoint configured

      // Store Ollama embedding configuration

        // Only remove embedding-specific lines, preserve general Ollama config

        // Check if we need to update the endpoint

          // Endpoint exists, only update if different
          // No existing endpoint, add it

        // Add embedding-specific configuration

/**
 * Prompts the user for Ollama configuration, validates it, and stores it
 * @param envFilePath Path to the .env file
 * @returns The configured Ollama settings or null if user cancels
 */

/**
 * Prompts the user for a Google Generative AI API key, validates it, and stores it
 * @param envFilePath Path to the .env file
 * @returns The configured Google API key or null if user cancels
 */
std::future<std::string> promptAndStoreGoogleKey(const std::string& envFilePath);

/**
 * Validates an OpenRouter API key format
 * @param key The API key to validate
 * @returns True if the key appears to be in valid format
 */
bool isValidOpenRouterKey(const std::string& key);

/**
 * Stores OpenRouter API key in the .env file
 * @param key The API key to store
 * @param envFilePath Path to the .env file
 */
std::future<void> storeOpenRouterKey(const std::string& key, const std::string& envFilePath);

/**
 * Prompts the user for an OpenRouter API key, validates it, and stores it
 * @param envFilePath Path to the .env file
 * @returns The configured OpenRouter API key or null if user cancels
 */
std::future<std::string> promptAndStoreOpenRouterKey(const std::string& envFilePath);

/**
 * Configures the database to use, either PGLite or PostgreSQL
 * @param reconfigure If true, force reconfiguration even if already configured
 * @returns The postgres URL if using Postgres, otherwise null
 */
std::future<std::string> configureDatabaseSettings(auto reconfigure);

// Main config schema
/**
 * Schema definition for the raw configuration object.
 *
 * @type {z.ZodType<RawConfig>}
 */

/**
 * Type definition for the inferred type of the raw config schema.
 */
using RawConfig = z::infer<typeof rawConfigSchema>;

/**
 * Define the type `Config` as the inferred type from the `configSchema`.
 */
using Config = z::infer<typeof configSchema>;

/**
 * Resolves the paths in the given configuration based on the provided current working directory (cwd).
 * @param {string} cwd - The current working directory.
 * @param {RawConfig} config - The raw configuration object.
 * @returns {Promise<Config>} The resolved configuration object with updated paths.
 */
std::future<void> resolveConfigPaths(const std::string& cwd, RawConfig config);

/**
 * Load environment variables from the project `.env` file if it exists.
 *
 * @param projectDir - Directory containing the `.env` file. Defaults to the current working directory.
 */

} // namespace elizaos
