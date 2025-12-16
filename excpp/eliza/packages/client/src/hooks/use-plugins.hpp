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



// Registry configuration - centralized for maintainability

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
 * Function to fetch plugins data from the registry API and merge with agent plugins.
 * @returns {UseQueryResult<PluginEntry[]>} Query result containing array of plugin entries
 */
void usePlugins();

} // namespace elizaos
