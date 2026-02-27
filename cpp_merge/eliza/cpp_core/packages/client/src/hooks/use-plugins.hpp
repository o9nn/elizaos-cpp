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



// Registry configuration - centralized for maintainability

struct GitVersionInfo {
    std: version;
    std: branch;
};

struct PluginGitInfo {
    std: repo;
    GitVersionInfo v0;
    GitVersionInfo v1;
};

struct PluginNpmInfo {
    std: repo;
    std: v0;
    std: v1;
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
};

/**
 * Function to fetch plugins data from the registry API and merge with agent plugins.
 * @returns {UseQueryResult<PluginEntry[]>} Query result containing array of plugin entries
 */
void usePlugins();

} // namespace elizaos
