#pragma once
#include <algorithm>
#include <any>
#include <cstdint>
#include <functional>
#include <future>
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
 * Validate plugin requirements
 */
std::future<void> validatePluginRequirements(const std:& cwd, PackageJson packageJson);

/**
 * Check if user is a maintainer for the package
 */
bool isMaintainer(PackageJson packageJson, const std:& username);

/**
 * Display appropriate registry publication messaging based on options and user status
 */
void displayRegistryPublicationMessage(std::optional<std:> opts, bool userIsMaintainer, std::optional<std:> registryPrUrl);

} // namespace elizaos
