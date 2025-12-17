#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "...utils/bun-exec.js.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Get or prompt for NPM username and ensure authentication
 */
std::future<std::string> getNpmUsername();

} // namespace elizaos
