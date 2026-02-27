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
#include "utils/directory.hpp"
#include "utils/naming.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Character updater imports removed - reverting to project-scoped plugins

/**
 * Remove a plugin from the project
 */
std::future<void> removePlugin(const std::string& plugin);

} // namespace elizaos
