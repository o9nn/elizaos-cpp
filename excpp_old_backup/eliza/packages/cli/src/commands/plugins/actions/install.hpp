#include ".types.hpp"
#include ".utils/directory.hpp"
#include ".utils/env-vars.hpp"
#include ".utils/naming.hpp"
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



// Character updater imports removed - reverting to project-scoped plugins

/**
 * Install a plugin from GitHub repository
 */
std::future<void> installPluginFromGitHub(const std::string& plugin, const std::string& cwd, AddPluginOptions opts);

/**
 * Install a plugin from registry
 */
std::future<void> installPluginFromRegistry(const std::string& plugin, const std::string& cwd, AddPluginOptions opts);

/**
 * Main plugin installation function
 */
std::future<void> addPlugin(const std::string& pluginArg, AddPluginOptions opts);

} // namespace elizaos
