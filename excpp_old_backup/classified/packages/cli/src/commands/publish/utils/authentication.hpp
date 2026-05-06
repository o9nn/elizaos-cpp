#include "...utils/bun-exec.js.hpp"
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
 * Get or prompt for NPM username and ensure authentication
 */
std::future<std::string> getNpmUsername();

} // namespace elizaos
