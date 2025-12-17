#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".types.hpp"
#include "elizaos/core.hpp"
#include "version-utils.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Check for available updates
 */
std::future<UpdateCheckResult> checkForUpdates(const std::unordered_map<std::string, std::string>& dependencies);

/**
 * Display update summary
 */
void displayUpdateSummary(const std::unordered_map<std::string, PackageUpdate>& updates);

/**
 * Update package.json with new versions
 */
std::future<void> updatePackageJson(const std::string& packageJsonPath, const std::unordered_map<std::string, PackageUpdate>& updates);

/**
 * Install dependencies using the detected package manager
 */
std::future<void> installDependencies(const std::string& cwd);

} // namespace elizaos
