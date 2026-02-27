#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "types.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Get platform-specific Bun installation instructions
 */
PlatformInstructions getBunInstallInstructions();

/**
 * Display the basic setup steps after cloning
 */
void displayBasicSteps(const std::string& cdPath);

/**
 * Display prerequisites and Bun installation guidance
 */
void displayPrerequisites();

/**
 * Display Bun installation instructions for the current platform
 */
void displayBunInstructions();

/**
 * Display complete next step instructions after cloning
 *
 * Shows setup steps, prerequisites, and platform-specific installation guidance.
 */
void displayNextSteps(const std::string& targetDir);

} // namespace elizaos
