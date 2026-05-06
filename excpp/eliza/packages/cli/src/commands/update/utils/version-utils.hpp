#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "elizaos/core.hpp"
#include "types.hpp"

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
std::future<std::string> fetchLatestVersion(const std::string& packageName);

} // namespace elizaos
