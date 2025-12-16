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
 * Clones a GitHub repository at a specified branch into a target directory.
 *
 * @param repo - The GitHub repository in "owner/repo" shorthand or full URL.
 * @param branch - The branch to clone from the repository.
 * @param destination - The directory where the repository will be cloned.
 *
 * @throws {Error} If the specified branch does not exist in the repository.
 * @throws {Error} If cloning fails for any other reason.
 */
std::future<void> cloneRepository(const std::string& repo, const std::string& branch, const std::string& destination);

/**
 * Prepares the destination directory for cloning
 *
 * Creates the directory if it doesn't exist, or validates that it's empty if it does exist.
 */
std::string prepareDestination(const std::string& dir);

/**
 * Main monorepo cloning action
 *
 * Handles the complete cloning process including directory preparation and error handling.
 */
std::future<void> cloneMonorepo(CloneInfo cloneInfo);

} // namespace elizaos
