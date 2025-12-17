#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "...utils/bun-exec.js.hpp"
#include "..update.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Check if the current CLI version is up to date
 */
std::future<std::string> checkCliVersion();

} // namespace elizaos
