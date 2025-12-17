#pragma once
#include <any>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "..env/utils/file-operations.hpp"
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Load environment configuration for runtime
 *
 * Loads environment variables from the project's .env file and returns them as runtime settings.
 */
std::future<RuntimeSettings> loadEnvConfig();

/**
 * Validates if a character has secrets configured
 * @param character - Character configuration object
 * @returns boolean - true if character has secrets, false otherwise
 */
bool hasCharacterSecrets(const std::any& character);

/**
 * Ensures character has a settings object
 * @param character - Character configuration object
 */
void ensureCharacterSettings(const std::any& character);

/**
 * Loads secrets from local .env file
 * @returns Promise<Record<string, string> | null> - env vars or null if no .env found
 */

/**
 * Sets default secrets from local .env if character doesn't have any
 * @param character - Character configuration object
 * @returns Promise<boolean> - true if secrets were added, false otherwise
 */
std::future<bool> setDefaultSecretsFromEnv(const std::any& character);

} // namespace elizaos
