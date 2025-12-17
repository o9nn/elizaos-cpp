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
    std::optional<{> elizaos;
    std::optional<std::vector<PluginSecret>> secrets;
    std::optional<string[]; // Legacy format - just array of secret names> requiredSecrets;
    std::optional<{> agentConfig;
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
    string | null version;
    string | null branch;
};

struct PluginGitInfo {
    std::string repo;
    GitVersionInfo v0;
    GitVersionInfo v1;
};

struct PluginNpmInfo {
    std::string repo;
    string | null v0;
    string | null v1;
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
    std::unordered_map<std::string, PluginInfo> registry;
};

/**
 * Fetch the plugin registry to get GitHub repo information
 */
std::future<RegistryResponse | null> fetchPluginRegistry();

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

/**
 * Fetches package.json for a single plugin from GitHub
 */
std::future<PluginPackageJson | null> fetchPluginPackageJson(const std::string& pluginName, string | null repoPath);

/**
 * Extract required secrets from package.json
 */
std::vector<PluginSecret> extractRequiredSecrets(const std::string& pluginName, PluginPackageJson | null packageJson);

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
void validateRequiredSecrets(const std::vector<PluginSecret>& requiredSecrets, Record<string providedSecrets, auto string | null>); {


} // namespace elizaos
