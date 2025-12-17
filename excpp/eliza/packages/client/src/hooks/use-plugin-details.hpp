#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Registry configuration - same as in use-plugins.ts

// Define the structure of plugin secrets requirements
struct PluginSecret {
    std::string name;
    std::optional<std::string> description;
    bool required;
    std::optional<std::string> example;
};

struct PluginPackageJson {
    std::string name;
    std::string version;
    std::optional<std::string> description;
    std::optional<std::vector<PluginSecret>> secrets;
    std::optional<std::vector<std::string>> requiredSecrets;
    std::optional<std::string> pluginType;
    std::optional<Record<> pluginParameters;
    std::string type;
    std::optional<std::string> description;
    bool required;
    std::optional<bool> sensitive;
    std::optional<std::string> example;
};

struct PluginDetails {
    std::string name;
    std::vector<PluginSecret> requiredSecrets;
};

// Core plugins that are part of the monorepo and don't need external fetching

// Registry types (same as in use-plugins.ts)
struct GitVersionInfo {
    std::string version;
    std::string branch;
};

struct PluginGitInfo {
    std::string repo;
    GitVersionInfo v0;
    GitVersionInfo v1;
};

struct PluginNpmInfo {
    std::string repo;
    std::string v0;
    std::string v1;
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
    std::string lastUpdatedAt;
};

/**
 * Fetch the plugin registry to get GitHub repo information
 */

/**
 * Convert plugin name for registry lookup - handles both @elizaos and @elizaos-plugins formats
 */
std::string getRegistryPluginName(const std::string& pluginName);

/**
 * Check if a plugin is a core plugin that doesn't need external fetching
 */
bool isCorePlugin(const std::string& pluginName);

/**
 * Get GitHub repo path from registry data
 */
std::string getGitHubRepoPath(const std::string& pluginName, const std::optional<RegistryResponse>& registryData);

/**
 * Fetches package.json for a single plugin from GitHub
 */

/**
 * Extract required secrets from package.json
 */
std::vector<PluginSecret> extractRequiredSecrets(const std::string& pluginName, const std::optional<PluginPackageJson>& packageJson);

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


} // namespace elizaos
