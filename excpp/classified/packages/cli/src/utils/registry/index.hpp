#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".bun-exec.js.hpp"
#include "constants.hpp"
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Use resolveEnvFile to match how credentials are saved, with fallback to ~/.eliza/.env

struct RegistrySettings {
    std::string defaultRegistry;
    std::string registry;
    std::optional<std::string> username;
    std::optional<bool> useNpm;
    std::optional<std::string> platform;
};

struct DataDirStatus {
    bool exists;
    bool exists;
    std::vector<std::string> missingKeys;
    bool hasAllKeys;
    bool exists;
    std::vector<std::string> missingKeys;
    bool hasAllKeys;
};

std::future<void> ensureElizaDir();

std::future<RegistrySettings> getRegistrySettings();

std::future<void> saveRegistrySettings(RegistrySettings settings);

std::future<std::string> getEnvVar(const std::string& key);

std::future<void> setEnvVar(const std::string& key, const std::string& value);

std::future<std::string> getGitHubToken();

std::future<void> setGitHubToken(const std::string& token);

/**
 * Normalizes a package name by removing scope prefixes
 * @param packageName The package name to normalize
 * @returns The normalized package name without scope prefix
 */
std::string normalizePackageName(const std::string& packageName);

struct PluginMetadata {
    std::string name;
    std::string description;
    std::string author;
    std::string repository;
    std::vector<std::string> versions;
    std::string latestVersion;
    std::string runtimeVersion;
    std::string maintainer;
    std::optional<std::vector<std::string>> tags;
    std::optional<std::vector<std::string>> categories;
};

// Default registry data for offline use or when GitHub is unavailable

/**
 * Saves the registry index to the cache file
 */
std::future<void> saveRegistryCache(const std::unordered_map<std::string, std::string>& registry);

/**
 * Gets a local copy of the registry index without requiring GitHub authentication.
 * This is useful for offline mode or when GitHub is unavailable.
 *
 * @returns {Promise<Record<string, string>>} The local registry index
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
 * @returns {string[]} An array of possible normalized plugin names to try
 */
std::vector<std::string> normalizePluginName(const std::string& pluginName);

/**
 * Retrieves the repository URL for a given plugin from the registry.
 *
 * @param {string} pluginName - The name of the plugin to fetch the repository URL for.
 * @returns {Promise<string | null>} The repository URL for the plugin, or null if not found.
 * @throws {Error} If an error occurs while retrieving the repository URL.
 */
std::future<std::string> getPluginRepository(const std::string& pluginName);

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
std::future<bool> repoHasBranch(const std::string& repoUrl, const std::string& branchName);

std::future<std::string> getBestBranch(const std::string& repoUrl);

std::future<std::string> getPluginVersion(const std::string& pluginName, std::optional<std::string> version);

/**
 * Attempts to get package details from the registry
 */
    // Normalize the package name (remove prefix if present)

    // Get package details from registry

    // Use agent only if https_proxy is defined
      // @ts-ignore - HttpsProxyAgent is not in the RequestInit type

    // Get the response body

/**
 * Gets the best matching version of a plugin based on runtime version
 */
std::future<std::string> getBestPluginVersion(const std::string& packageName, const std::string& runtimeVersion);

std::future<DataDirStatus> checkDataDir();

std::future<void> initializeDataDir();

std::future<bool> validateDataDir();

} // namespace elizaos
