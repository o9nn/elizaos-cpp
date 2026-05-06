#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".types.hpp"
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Upgrade a plugin from version 0.x to 1.x using AI-powered migration with Claude Code SDK
 */
std::future<void> upgradePlugin(const std::string& pluginPath, UpgradePluginOptions opts);

} // namespace elizaos
