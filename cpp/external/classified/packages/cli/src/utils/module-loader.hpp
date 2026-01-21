#include "elizaos/core.hpp"
#include "user-environment.hpp"
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



/**
 * ModuleLoader provides a clean way to load modules from the project's local node_modules
 * instead of the global CLI's bundled dependencies. This solves singleton pattern issues
 * and ensures consistent module instances across the application.
 */
class ModuleLoader {
  private require: NodeRequire;
  private asyncCache = new Map<string, any>();
  private syncCache = new Map<string, any>();
  private projectPath: string;

  constructor(projectPath?: string) {
    this.projectPath = projectPath || this.detectProjectPath();
    // Create require function scoped to the project directory
    // This ensures module resolution starts from the project's package.json
    this.require = createRequire(pathToFileURL(path.join(this.projectPath, 'package.json')).href);

    // Set up environment with proper module resolution paths
    // This ensures the same local-first guarantees as server-manager.ts
    this.setupEnvironment();
  }

  /**
   * Detect the appropriate project path, preferring monorepo root over current working directory
   */
    // Try to detect monorepo root using existing UserEnvironment utility

    // Fallback to current working directory

  /**
   * Load a module from the project's node_modules directory.
   * Uses caching to ensure the same instance is returned for repeated calls.
   *
   * @param moduleName - The name of the module to load (e.g., '@elizaos/server')
   * @returns The loaded module
   * @throws Error if the module cannot be found in the project
   */
    // Return cached module if already loaded

      // First, explicitly check if local module exists (same as server-manager.ts logic)

      // Resolve the module path using project-scoped require

      // Verify we're actually using local module when available

      // Use dynamic import with file URL for cross-platform compatibility

      // Cache the loaded module

  /**
   * Synchronously load a module from the project's node_modules directory.
   * Uses caching to ensure the same instance is returned for repeated calls.
   *
   * @param moduleName - The name of the module to load (e.g., '@elizaos/server')
   * @returns The loaded module
   * @throws Error if the module cannot be found in the project
   */
    // Return cached module if already loaded

      // First, explicitly check if local module exists (same as server-manager.ts logic)

      // Resolve the module path using project-scoped require

      // Verify we're actually using local module when available

      // Use require for synchronous loading

      // Cache the loaded module

  /**
   * Set up environment with proper module resolution paths.
   * This ensures the same local-first guarantees as server-manager.ts.
   */
    // Add local node_modules to NODE_PATH for proper module resolution

    // Add local .bin to PATH to prioritize local executables

  /**
   * Clear the module cache. Useful for testing or hot reloading scenarios.
   */

  /**
   * Get the resolved path for a module without loading it.
   * Useful for debugging or verification.
   *
   * @param moduleName - The name of the module to resolve
   * @returns The resolved file path
   */

// Singleton instance for the current project

/**
 * Get the default module loader instance for the current project.
 * Creates a new instance if one doesn't exist.
 */
ModuleLoader getModuleLoader();

/**
 * Convenience function to load a module using the default loader.
 *
 * @param moduleName - The name of the module to load
 * @returns The loaded module
 */

/**
 * Convenience function to synchronously load a module using the default loader.
 *
 * @param moduleName - The name of the module to load
 * @returns The loaded module
 */

} // namespace elizaos
