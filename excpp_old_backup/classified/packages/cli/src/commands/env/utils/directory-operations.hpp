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
 * Delete a directory with error handling
 * @param dir Directory path to delete
 * @param actions Action log collection to update
 * @param label Description label for this operation
 * @returns Success or failure
 */
std::future<bool> safeDeleteDirectory(const std::string& dir, ResetActionRecord actions, const std::string& label);

} // namespace elizaos
