#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "bun-exec.hpp"
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Constants

/**
 * Updates the PATH environment variable to include Bun's installation directory
 */
void updatePathForBun();

/**
 * Checks if Bun is already installed
 */
std::future<bool> isBunInstalled();

/**
 * Attempts to automatically install Bun based on the platform
 */
std::future<bool> autoInstallBun();

/**
 * Determines if auto-installation should be attempted
 */
bool shouldAutoInstall();

} // namespace elizaos
