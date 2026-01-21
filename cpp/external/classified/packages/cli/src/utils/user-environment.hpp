#include "auto-install-bun.hpp"
#include "bun-exec.hpp"
#include "elizaos/core.hpp"
#include "emoji-handler.hpp"
#include "resolve-utils.hpp"
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



// Types
struct OSInfo {
    std::string platform;
    std::string release;
    std::string arch;
    std::string type;
    std::string version;
    std::string homedir;
};

struct CLIInfo {
    std::string version;
    std::string name;
    std::string path;
};

struct PackageManagerInfo {
    'bun' name;
    string | null version;
    bool global;
    bool isNpx;
    bool isBunx;
};

struct PathInfo {
    std::string elizaDir;
    std::string envFilePath;
    std::string configPath;
    std::string pluginsDir;
    string | null monorepoRoot;
    std::string packageJsonPath;
};

struct EnvInfo {
    std::optional<std::string> GITHUB_USERNAME;
    std::optional<std::string> GITHUB_TOKEN;
};

struct UserEnvironmentInfo {
    OSInfo os;
    CLIInfo cli;
    PackageManagerInfo packageManager;
    std::string timestamp;
    PathInfo paths;
    EnvInfo env;
};

/**
 * Provides information about the user's environment including OS, CLI, and package manager details.
 * Uses singleton pattern to cache results.
 */
class UserEnvironment {
  public static readonly getInstance = () => UserEnvironment.instance;

  public static readonly getInstanceInfo = () => UserEnvironment.instance.getInfo();

  private static readonly instance: UserEnvironment = new UserEnvironment();
  private cachedInfo: { [key: string]: UserEnvironmentInfo } = {}; // Cache per directory

  /**
   * Gets operating system information
   */

  /**
   * Gets CLI version and package information
   */

  /**
   * Detects the active package manager - always returns bun for ElizaOS CLI
   */

    // Check if running via bunx by looking for bunx cache patterns in the script path

    // First check if we're already running under Bun
        // Get bun version from command line

        // Attempt auto-installation if conditions are met

            // Try to get version again after installation
              // Continue to manual installation instructions

        // If auto-installation failed or was not attempted, show manual instructions

          // Force exit the process - Bun is required for ElizaOS CLI

    // First check if the script path indicates a global installation

      // Check if running via npx/bunx first, as these might trigger global check falsely
        // If we're already in a global path, consider it global
          // Check if bun has the CLI installed globally
          // Use Bun.spawnSync for checking global packages

      // Package not found globally - but still might be global based on path

    // Combine check with NODE_ENV check

  /**
   * Finds the monorepo root by traversing upwards from a starting directory,
   * looking for a marker directory ('packages/core').
   *
   * @param startDir The directory to start searching from.
   * @returns The path to the monorepo root if found, otherwise null.
   */

            // Additional validation: check if this looks like the ElizaOS monorepo
              // Verify this is actually the ElizaOS monorepo
          // Ignore errors like permission denied, continue search

        // Reached filesystem root

    // Reached max levels

    // Resolve .env from current working directory up to monorepo root (if any), or only cwd if not in monorepo

    // Return a copy of process.env as EnvInfo

  /**
   * Clears the cached information
   */

  /**
   * Gets the version of a specified package from monorepo, local dependencies, or npm
   */

      // Try monorepo first if available

      // Check CLI package dependencies

      // Try npm as last resort

  /**
   * Get local packages available in the monorepo
   */


} // namespace elizaos
