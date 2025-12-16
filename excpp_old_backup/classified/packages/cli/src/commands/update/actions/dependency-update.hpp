#include ".types.hpp"
#include ".utils/package-utils.hpp"
#include ".utils/version-utils.hpp"
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
 * Main dependency update function
 *
 * Updates ElizaOS dependencies in a project or plugin, with support for dry-run mode, major version confirmation, and optional build step.
 */
std::future<void> updateDependencies(const std::string& cwd, bool isPlugin, UpdateOptions = {} options);

} // namespace elizaos
