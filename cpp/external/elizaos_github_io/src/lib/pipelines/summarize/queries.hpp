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
#include "queryHelpers.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Get metrics for a contributor within a time range
 */
std::future<void> getContributorMetrics(auto dateRange);

/**
 * Get summaries for a list of contributors for a specific interval.
 */

} // namespace elizaos
