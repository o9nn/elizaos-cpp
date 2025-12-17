#include ".types.hpp"
#include ".utils/build-utils.hpp"
#include ".utils/file-watcher.hpp"
#include ".utils/server-manager.hpp"
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



/**
 * Start development mode with file watching and auto-restart
 *
 * Sets up a development environment with automatic rebuilding and server restarting when files change.
 */
std::future<void> startDevMode(DevOptions options);

} // namespace elizaos
