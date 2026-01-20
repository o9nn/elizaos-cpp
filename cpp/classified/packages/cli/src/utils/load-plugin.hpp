#include "elizaos/core.hpp"
#include "plugin-context.hpp"
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



struct PackageJson {
    std::optional<std::string> module;
    std::optional<std::string> main;
};

struct ImportStrategy {
    std::string name;
    (repository: string) => Promise<any | null> tryImport;
};

/**
 * Get the global node_modules path based on Node.js/Bun installation
 */
std::string getGlobalNodeModulesPath();

/**
 * Helper function to resolve a path within node_modules
 */
std::string resolveNodeModulesPath(const std::string& repository, const std::vector<std::string>& ...segments);

/**
 * Helper function to read and parse package.json
 */
std::future<PackageJson | null> readPackageJson(const std::string& repository);

/**
 * Normalizes import paths for cross-platform compatibility.
 * On Windows, converts absolute paths to file:// URLs for dynamic imports.
 */
std::string normalizeImportPath(const std::string& importPath);

/**
 * Attempts to import a module from a given path and logs the outcome.
 */
std::future<any | null> tryImporting(const std::string& importPath, const std::string& strategy, const std::string& repository);

/**
 * Collection of import strategies
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
 * Get relevant import strategies based on plugin type
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
std::future<any | null> loadPluginModule(const std::string& repository);

} // namespace elizaos
