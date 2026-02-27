#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
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
std::unordered_set<std:> getResolved(const std:& filePath);

/**
 * Get submitted instance IDs from results file
 */
std::unordered_set<std:> getSubmitted(const std:& filePath);

/**
 * Show statistics for a single run
 */
void statsSingle(const std:& filePath);

/**
 * Compare many runs
 */
void compareMany(const std::vector<std::string>& paths);

/**
 * Compare a std::pair of runs
 */
void comparePair(const std:& newPath, const std:& oldPath, bool showSame = false);

/**
 * Main std::function to compare runs
 */
std::future<void> compareRuns(const std::vector<std::string>& paths, bool showSame = false);

} // namespace elizaos
