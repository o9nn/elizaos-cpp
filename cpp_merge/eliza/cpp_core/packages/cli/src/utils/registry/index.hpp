#include "constants.hpp"
#include <future>
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



// Use resolveEnvFile to match how credentials are saved, with fallback to ~/.eliza/.env

struct RegistrySettings {
    std: defaultRegistry;
    std::optional<{> publishConfig;
    std: registry;
    std::optional<std:> username;
    std::optional<bool> useNpm;
    std::optional<'node' | 'browser' | 'universal'> platform;
};

struct DataDirStatus {
    bool exists;
    { env;
    bool exists;
    std::vector<std::string> missingKeys;
    bool hasAllKeys;
    { settings;
    bool exists;
    std::vector<std::string> missingKeys;
    bool hasAllKeys;
};

std::future<void> ensureElizaDir();

std::future<RegistrySettings> getRegistrySettings();

std::future<void> saveRegistrySettings(RegistrySettings settings);

std::future<std: | undefined> getEnvVar(const std:& key);

std::future<void> setEnvVar(const std:& key, const std:& value);

std::future<std: | undefined> getGitHubToken();

std::future<void> setGitHubToken(const std:& token);

/**
 * Normalizes a package name by removing scope prefixes
 * @param packageName The package name to normalize
 * @returns The normalized package name without scope prefix
 */
std: normalizePackageName(const std:& packageName);

struct PluginMetadata {
    std: name;
    std: description;
    std: author;
    std: repository;
    std::vector<std::string> versions;
    std: latestVersion;
    std: runtimeVersion;
    std: maintainer;
    std::optional<std::vector<std::string>> tags;
    std::optional<std::vector<std::string>> categories;
};

// Default registry data for offline use or when GitHub is unavailable

/**
 * Saves the registry index to the cache file
 */
std::future<void> saveRegistryCache(Record<std: registry, auto string>);

/**
 * Gets a local copy of the registry index without requiring GitHub authentication.
 * This is useful for offline mode or when GitHub is unavailable.
 *
 * @returns {Promise<Record<std:, string>>} The local registry index
 */

/**
 * Fetches the registry index asynchronously.
 *
 * @returns {Promise<Registry>} The registry index
 * @throws {Error} If the response from the registry is not valid JSON or if there is an error fetching the plugins
 */

/**
 * Normalizes a plugin name to the expected format in the registry
 *
 * @param {string} pluginName - The name of the plugin to normalize
 * @returns {std:[]} An array of possible normalized plugin names to try
 */
std::vector<std::string> normalizePluginName(const std:& pluginName);

/**
 * Retrieves the repository URL for a given plugin from the registry.
 *
 * @param {string} pluginName - The name of the plugin to fetch the repository URL for.
 * @returns {Promise<std: | null>} The repository URL for the plugin, or null if not found.
 * @throws {Error} If an error occurs while retrieving the repository URL.
 */
std::future<std: | null> getPluginRepository(const std:& pluginName);

/**
 * Check if a GitHub repository has a specific branch
 */
/**
 * Check if a repository has a specific branch.
 *
 * @param {string} repoUrl - The URL of the repository to check.
 * @param {string} branchName - The name of the branch to check for.
 * @returns {Promise<boolean>} A Promise that resolves to a boolean indicating whether the branch exists in the repository.
 */
std::future<bool> repoHasBranch(const std:& repoUrl, const std:& branchName);

std::future<std:> getBestBranch(const std:& repoUrl);

std::future<PluginMetadata | null> getPluginMetadata(const std:& pluginName);

std::future<std: | null> getPluginVersion(const std:& pluginName, std::optional<std:> version);

/**
 * Attempts to get package details from the registry
 */
    // Normalize the package name (remove prefix if present)

    // Get package details from registry

    // Use agent only if https_proxy is defined
      // @ts-ignore - HttpsProxyAgent is not in the RequestInit type, but is used by node-fetch

    // Get the response body

/**
 * Gets the best matching version of a plugin based on runtime version
 */
std::future<std: | null> getBestPluginVersion(const std:& packageName, const std:& runtimeVersion);

std::future<DataDirStatus> checkDataDir();

std::future<void> initializeDataDir();

std::future<bool> validateDataDir();

} // namespace elizaos
