#include ".types.hpp"
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
 * Validate plugin requirements
 */
std::future<void> validatePluginRequirements(const std::string& cwd, PackageJson packageJson);

/**
 * Check if user is a maintainer for the package
 */
bool isMaintainer(PackageJson packageJson, const std::string& username);

/**
 * Display appropriate registry publication messaging based on options and user status
 */
void displayRegistryPublicationMessage(std::optional<{ skipRegistry: boolean; npm: boolean }> opts, bool userIsMaintainer, std::optional<std::string> registryPrUrl);

} // namespace elizaos
