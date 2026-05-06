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
std::future<UpdateCheckResult> checkForUpdates(Record<string dependencies, auto string>);

/**
 * Display update summary
 */
void displayUpdateSummary(Record<string updates, auto PackageUpdate>);

/**
 * Update package.json with new versions
 */
std::future<void> updatePackageJson(const std::string& packageJsonPath, Record<string updates, auto PackageUpdate>);

/**
 * Install dependencies using the detected package manager
 */
std::future<void> installDependencies(const std::string& cwd);

} // namespace elizaos
