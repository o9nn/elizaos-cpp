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
 * Utility to categorize a work item (PR title or commit message)
 *
 * @param text - The PR title or commit message to categorize
 * @returns The category of the work item
 */
WorkItemType categorizeWorkItem(const std::string& text);

/**
 * Utility to extract area from a file path
 */

/**
 * Check if a file is a common root configuration file that should be ignored
 */
bool isRootConfigFile(const std::string& path);

using WorkItemType = std::variant<, "feature", "bugfix", "refactor", "docs", "tests", "other">;

/**
 * Utility to build a map of focus areas from files
 */
std::unordered_map<std::string, double> buildAreaMap(std::optional<std::vector<{ path: string; filename: string }>> files);

} // namespace elizaos
