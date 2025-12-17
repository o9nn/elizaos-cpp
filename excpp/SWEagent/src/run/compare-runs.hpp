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
 * Compare multiple run results
 */

/**
 * Get resolved instance IDs from results file
 */
std::unordered_set<std::string> getResolved(const std::string& filePath);

/**
 * Get submitted instance IDs from results file
 */
std::unordered_set<std::string> getSubmitted(const std::string& filePath);

/**
 * Show statistics for a single run
 */
void statsSingle(const std::string& filePath);

/**
 * Compare many runs
 */
void compareMany(const std::vector<std::string>& paths);

/**
 * Compare a pair of runs
 */
void comparePair(const std::string& newPath, const std::string& oldPath, boolean = false showSame);

/**
 * Main function to compare runs
 */
std::future<void> compareRuns(const std::vector<std::string>& paths, boolean = false showSame);

} // namespace elizaos
