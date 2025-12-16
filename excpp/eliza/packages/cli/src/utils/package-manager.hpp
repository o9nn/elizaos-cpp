#include "bun-installation-helper.hpp"
#include "elizaos/core.hpp"
#include "run-bun.hpp"
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
 * Always returns 'bun' as the package manager for ElizaOS CLI.
 *
 * @returns A promise that resolves to 'bun'.
 */
std::future<std::string> getPackageManager();

/**
 * Check if the CLI is running from a global installation
 * @returns {boolean} - Whether the CLI is globally installed
 */
std::future<bool> isGlobalInstallation();

/**
 * Check if we're running via npx
 * @returns {boolean} - Whether we're running through npx
 */
std::future<bool> isRunningViaNpx();

/**
 * Check if we're running via bunx
 * @returns {boolean} - Whether we're running through bunx
 */
std::future<bool> isRunningViaBunx();

/**
 * Get the install command for bun
 * @param {boolean} isGlobal - Whether to install globally
 * @returns {string[]} - The bun install command array
 */
std::vector<std::string> getInstallCommand(bool isGlobal);

/**
 * Removes a package from bun.lock file to prevent circular dependency issues
 * @param packageName - The package name to remove from lockfile
 * @param directory - The directory containing the bun.lock file
 */
std::future<void> removeFromBunLock(const std::string& packageName, const std::string& directory);

/**
 * Installs a package using the appropriate package manager, attempting multiple strategies if necessary.
 *
 * Tries to install the specified package from the npm registry, GitHub repositories, or a monorepo, based on the provided options and available sources. Handles normalization of plugin package names and supports version or tag specification.
 *
 * @param packageName - The name of the package to install. Can be a scoped package, organization/repo, or plugin name.
 * @param versionOrTag - Optional version or tag to install. If omitted, installs the latest version.
 * @param directory - The directory in which to run the installation.
 * @param options - Optional settings to control which installation strategies to attempt and monorepo details.
 * @returns A promise resolving to an object indicating whether installation succeeded and the installed package identifier, or null if all methods failed.
 */

    await runBunCommand(args, directory);

    // Check if it's a bun not found error

/**
 * Builds a GitHub specifier string for package installation.
 *
 * @param githubSpec - The GitHub specifier (e.g., "github:owner/repo")
 * @param versionOrTag - Optional version or tag to append
 * @returns The complete GitHub specifier string
 */
std::string buildGitHubSpecifier(const std::string& githubSpec, std::optional<std::string> versionOrTag);

/**
 * Enhanced installation function that supports GitHub fallback with lockfile cleanup.
 *
 * @param packageName - The name of the package to install
 * @param versionOrTag - Optional version or tag to install
 * @param directory - The directory in which to run the installation
 * @param githubFallback - Optional GitHub repository path for fallback (e.g., "owner/repo")
 * @returns A promise resolving to an object indicating success and installed identifier
 */
  // First try normal installation

  // If npm installation failed and we have a GitHub fallback, try GitHub installation

  // Remove package from lockfile to prevent circular dependencies
  await removeFromBunLock(packageName, directory);

  // Try GitHub installation

} // namespace elizaos
