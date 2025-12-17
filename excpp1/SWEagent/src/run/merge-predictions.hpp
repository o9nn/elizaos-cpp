#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".utils/log.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * Merge multiple predictions into a single file
 */

/**
 * Merge predictions found in directories into a single JSON file
 */
void mergePredictions(const std::vector<std::string>& directories, std::optional<std::string> output);

/**
 * Recursively find all .pred files in a directory
 */
std::vector<std::string> findPredFiles(const std::string& directory);

} // namespace elizaos
