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

import type { PackageJson } from 'type-fest';
;

/**
 * Get the current version of a package from the monorepo
 */
/**
 * Retrieves the version of a specified package.
 *
 * @param {string} packageName - The name of the package to retrieve the version for.
 * @returns {Promise<string>} A promise that resolves with the version of the package.
 */
std::future<std::string> getPackageVersion(const std::string& packageName);

/**
 * Get local packages available in the monorepo
 */
std::future<std::vector<std::string>> getLocalPackages();

std::future<PackageJson> getPackageInfo(); = await UserEnvironment.getInstance().getPathInfo();
  try {
    const fileContent = await import('node:fs/promises').then((fs) =>
      fs.readFile(packageJsonPath, 'utf8')
    );
    return JSON.parse(fileContent) as PackageJson;
  } catch (error) {
    if (error instanceof Error) {
      // Check for file not found error (ENOENT)
      if ((error as NodeJS.ErrnoException).code === 'ENOENT') {
        throw new Error(`Error: Could not find package.json at ${packageJsonPath}`);
      }
      // Check for JSON parsing error
      if (error instanceof SyntaxError) {
        throw new Error(
          `Error: Invalid JSON in package.json at ${packageJsonPath}. Details: ${error.message}`
        );
      }
      // Rethrow other errors
      throw new Error(
        `Error reading or parsing package.json at ${packageJsonPath}: ${error.message}`
      );
    }
    // Fallback for non-Error objects thrown
    throw new Error(
      `An unexpected error occurred while reading or parsing package.json at ${packageJsonPath}`
    );
  }
}

} // namespace elizaos
