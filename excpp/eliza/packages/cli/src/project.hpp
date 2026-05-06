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

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Interface for a project module that can be loaded.
 */
struct ProjectModule {
    std::optional<std::vector<ProjectAgent>> agents;
    std::optional<Character> character;
};

/**
 * Interface for a loaded project.
 */
struct Project {
    std::vector<ProjectAgent> agents;
    std::string dir;
    std::optional<bool> isPlugin;
    std::optional<Plugin> pluginModule;
};

struct LoadedProject {
    std::vector<AgentRuntime> runtimes;
    std::string path;
    std::vector<ProjectAgent> agents;
};

/**
 * Determine if a loaded module is a plugin
 * @param module The loaded module to check
 * @returns true if this appears to be a plugin
 */
bool isPlugin(const std::any& module);

/**
 * Extract a Plugin object from a module
 * @param module The module to extract from
 * @returns The plugin object
 */
Plugin extractPlugin(const std::any& module);

/**
 * Loads a project from the specified directory.
 * @param {string} dir - The directory to load the project from.
 * @returns {Promise<Project>} A promise that resolves to the loaded project.
 */
std::future<Project> loadProject(const std::string& dir);

} // namespace elizaos
