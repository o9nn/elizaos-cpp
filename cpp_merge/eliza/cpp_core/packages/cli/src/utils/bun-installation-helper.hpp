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
#include "emoji-handler.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Display helpful bun installation instructions with OS-specific commands
 */
void displayBunInstallationTips();

/**
 * Display compact bun installation tip for inline use
 */
std::string displayBunInstallationTipCompact();

/**
 * Check if bun is available and provide installation tips if not
 */
std::future<bool> ensureBunAvailable();

} // namespace elizaos
