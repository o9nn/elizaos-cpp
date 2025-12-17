#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "elizaos/core.hpp"
#include "emoji-handler.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Automatically install Bun if it's not available
 * @returns {Promise<boolean>} - Whether Bun is now available
 */
std::future<bool> autoInstallBun();

/**
 * Check if auto-installation should be attempted
 * @returns {boolean} - Whether to attempt auto-installation
 */
bool shouldAutoInstall();

} // namespace elizaos
