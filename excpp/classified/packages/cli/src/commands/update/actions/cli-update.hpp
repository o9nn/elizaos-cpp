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
#include "utils/version-utils.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Update CLI to latest version
 *
 * Handles CLI updates with automatic migration from npm to bun when appropriate, and supports both global and local installation scenarios.
 */
std::future<bool> performCliUpdate(GlobalUpdateOptions options = {});

} // namespace elizaos
