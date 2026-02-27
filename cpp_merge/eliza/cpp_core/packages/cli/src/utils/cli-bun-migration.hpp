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

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Check if bun is available on the system
 */
std::future<bool> isBunAvailable();

/**
 * Check if the CLI was installed via npm globally
 * Handles multiple node versions (nvm scenarios)
 */
std::future<bool> isCliInstalledViaNpm();

/**
 * Remove the CLI from npm global installation
 */
std::future<void> removeNpmInstallation();

/**
 * Install the CLI using bun globally
 */
std::future<void> installCliWithBun(const std::string& version);

/**
 * Verify the CLI installation works and returns expected version
 */
std::future<bool> verifyCliInstallation(const std::string& expectedVersion);

/**
 * Atomic migration: CLI from npm to bun installation
 * Installs bun version first, only removes npm if successful
 */
std::future<void> migrateCliToBun(const std::string& targetVersion);

} // namespace elizaos
