#include ".types.hpp"
#include "elizaos/core.hpp"
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



// Constants

/**
 * Get current CLI version using UserEnvironment
 */
std::future<std::string> getVersion();

/**
 * Check if version string is a workspace reference
 */

/**
 * Check if version is a special tag
 */

/**
 * Version comparison helper
 */
VersionCheckResult checkVersionNeedsUpdate(const std::string& currentVersion, const std::string& targetVersion);

/**
 * Check for major version update
 */
bool isMajorUpdate(const std::string& currentVersion, const std::string& targetVersion);

/**
 * Fetch latest package version from npm registry
 */
std::future<string | null> fetchLatestVersion(const std::string& packageName);

} // namespace elizaos
