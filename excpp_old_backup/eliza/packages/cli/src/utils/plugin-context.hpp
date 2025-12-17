#include "build-project.hpp"
#include "directory-detection.hpp"
#include "elizaos/core.hpp"
#include "registry.hpp"
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



struct PackageInfo {
    std::string name;
    std::optional<std::string> main;
    std::optional<std::unordered_map<std::string, std::string>> scripts;
};

struct PluginContext {
    bool isLocalDevelopment;
    std::optional<std::string> localPath;
    std::optional<PackageInfo> packageInfo;
    std::optional<bool> needsBuild;
};

/**
 * Normalizes plugin names for comparison by removing common prefixes and scopes
 */
std::string normalizeForComparison(const std::string& name);

/**
 * Detects if the current directory is the same plugin being requested
 * and provides context about local development status
 */
PluginContext detectPluginContext(const std::string& pluginName);

/**
 * Ensures a local plugin is built before attempting to load it
 */
std::future<bool> ensurePluginBuilt(PluginContext context);

/**
 * Provides helpful guidance when local plugin loading fails
 */
void provideLocalPluginGuidance(const std::string& pluginName, PluginContext context);

} // namespace elizaos
