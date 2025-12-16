#pragma once
#include <any>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".types.hpp"

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
void displayRegistryPublicationMessage(std::optional<std::any> opts, bool userIsMaintainer, std::optional<std::string> registryPrUrl);

} // namespace elizaos
