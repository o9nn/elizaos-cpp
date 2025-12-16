#include ".utils/log.hpp"
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
 * Calculate statistics from .traj files
 */

/**
 * Calculate statistics from .traj files
 */
std::future<std::string> quickStats(string = '.' directory);

/**
 * Recursively find all .traj files in a directory
 */
std::vector<std::string> findTrajFiles(const std::string& directory);

} // namespace elizaos
