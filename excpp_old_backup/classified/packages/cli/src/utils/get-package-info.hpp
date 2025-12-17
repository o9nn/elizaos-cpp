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

std::future<PackageJson> getPackageInfo();

} // namespace elizaos
