#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Determines the project type using comprehensive directory detection
 */
DirectoryInfo getProjectType(std::optional<std::string> testPath);

// Note: findMonorepoRoot() has been removed - use UserEnvironment.getInstance().findMonorepoRoot() instead
// This centralized version looks for packages/core directory instead of lerna.json for better reliability

/**
 * Process filter name to remove extensions consistently
 *
 * Note: Test filtering works in two ways:
 * 1. Matching test suite names (the string in describe() blocks)
 * 2. Matching file names (without extension)
 *
 * For best results, use the specific test suite name you want to run.
 * The filter is applied case-insensitively for better user experience.
 */
std::string processFilterName(std::optional<std::string> name);

/**
 * Install plugin dependencies for testing
 */
std::future<void> installPluginDependencies(DirectoryInfo projectInfo);

} // namespace elizaos
