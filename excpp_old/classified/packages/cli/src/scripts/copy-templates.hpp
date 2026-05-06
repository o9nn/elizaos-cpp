#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use


/**
 * This script copies template packages from the monorepo into the CLI templates directory
 * It runs before the CLI build to prepare templates that will be included in the distribution
 */

// Define paths

/**
 * Updates package.json with the CLI version and replaces workspace references
 */
std::future<void> updatePackageJson(const std::string& packagePath, const std::string& cliVersion);

std::future<void> main();


} // namespace elizaos
