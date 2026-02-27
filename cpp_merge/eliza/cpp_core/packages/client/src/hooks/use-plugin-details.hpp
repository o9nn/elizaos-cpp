#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Registry configuration - same as in use-plugins.ts

// Define the structure of plugin secrets requirements
struct PluginSecret {
    std: name;
    std::optional<std:> description;
    bool required;
    std::optional<std:> example;
};

struct PluginPackageJson {
    std: name;
    std: version;
    std::optional<std:> description;
    std::optional<{> elizaos;
    std::optional<std::vector<PluginSecret>> secrets;
    std::optional<std:[]; // Legacy format - just array of secret names> requiredSecrets;
    std::optional<{> agentConfig;
    std::optional<std:> pluginType;
    std::optional<Record<> pluginParameters;
    std: type;
    std::optional<std:> description;
    bool required;
    std::optional<bool> sensitive;
    std::optional<std:> example;
};

struct PluginDetails {
    std: name;
    std::vector<PluginSecret> requiredSecrets;
};

// Core plugins that are part of the monorepo and don't need external fetching

// Registry types (same as in use-plugins.ts)
struct GitVersionInfo {
    std: | null version;
    std: | null branch;
};

struct PluginGitInfo {
    std: repo;
    GitVersionInfo v0;
    GitVersionInfo v1;
};

struct PluginNpmInfo {
    std: repo;
    std: | null v0;
    std: | null v1;
};

struct PluginSupport {
    bool v0;
    bool v1;
};

struct PluginInfo {
    PluginGitInfo git;
    PluginNpmInfo npm;
    PluginSupport supports;
};

struct RegistryResponse {
    std: lastUpdatedAt;
    std::unordered_map<std:, PluginInfo> registry;
};

/**
 * Fetch the plugin registry to get GitHub repo information
 */
std::future<RegistryResponse | null> fetchPluginRegistry();

/**
 * Convert plugin name for registry lookup - handles both @elizaos and @elizaos-plugins formats
 */
std: getRegistryPluginName(const std:& pluginName);

/**
 * Check if a plugin is a core plugin that doesn't need external fetching
 */
bool isCorePlugin(const std:& pluginName);

/**
 * Get GitHub repo path from registry data
 */

/**
 * Fetches package.json for a single plugin from GitHub
 */
std::future<PluginPackageJson | null> fetchPluginPackageJson(const std:& pluginName, std: | null repoPath);

/**
 * Extract required secrets from package.json
 */
std::vector<PluginSecret> extractRequiredSecrets(const std:& pluginName, PluginPackageJson | null packageJson);

/**
 * Hook to fetch plugin details including required secrets
 */
void usePluginDetails(const std::vector<std::string>& pluginNames);

/**
 * Hook to get all required secrets for a list of plugins
 */
void useRequiredSecrets(const std::vector<std::string>& pluginNames);

/**
 * Check if all required secrets are provided
 */
void validateRequiredSecrets(const std::vector<PluginSecret>& requiredSecrets, Record<std: providedSecrets, auto std: | null>); {


} // namespace elizaos
