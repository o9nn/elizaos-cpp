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
#include "elizaos/core.hpp"
#include "plugin-context.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct PackageJson {
    std::optional<std::string> module;
    std::optional<std::string> main;
};

struct ImportStrategy {
    std::string name;
};

/**
 * Get the global node_modules path based on Node.js installation
 */
std::string getGlobalNodeModulesPath();

/**
 * Helper function to resolve a path within node_modules
 */
std::string resolveNodeModulesPath(const std::string& repository);

/**
 * Helper function to read and parse package.json
 */

/**
 */

/**
 */
  // Try local development first - this is the most important for plugin testing

        // Ensure the plugin is built

        // Try to load from built output
          return tryImporting(context.localPath, 'local development plugin', repository);

        // This shouldn't happen if ensurePluginBuilt succeeded, but handle it gracefully

  // Try workspace dependencies (for monorepo packages)
        // Try to find the plugin in the workspace
          return tryImporting(workspacePath, 'workspace dependency', repository);
      return tryImporting(globalPath, 'global node_modules', repository);

/**
 * Determines if a package name is from the ElizaOS ecosystem
 */
bool isElizaOSPackageName(const std::string& repository);

/**
 */
std::vector<ImportStrategy> getStrategiesForPlugin(const std::string& repository);

/**
 * Attempts to load a plugin module using relevant strategies based on plugin type.
 * ElizaOS ecosystem plugins (@elizaos/*) use all strategies,
 * while third-party plugins use only relevant strategies to avoid noise.
 *
 * @param repository - The plugin repository/package name to load.
 * @returns The loaded plugin module or null if loading fails after all attempts.
 */

} // namespace elizaos
