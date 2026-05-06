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
#include "types.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Build a single package
 */
std::future<BuildResult> buildPackage(const std::string& packagePath, bool isPlugin);

/**
 * Build core packages in a monorepo
 */

/**
 * Perform a full rebuild based on the development context
 *
 * Handles building in different contexts: monorepo, project, or plugin.
 */
std::future<void> performRebuild(DevContext context);

/**
 * Perform initial build setup
 */
std::future<void> performInitialBuild(DevContext context);

/**
 * Create development context from current working directory
 */
DevContext createDevContext(const std::string& cwd);

} // namespace elizaos
