#pragma once
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>
#include "aiContributorSummary.hpp"
#include "context.hpp"
#include "generateTimeIntervals.hpp"
#include "getActiveContributors.hpp"
#include "mutations.hpp"
#include "queries.hpp"
#include "types.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Check if a summary already exists for a user on a specific date and interval type
 */
std::future<bool> checkExistingSummary(const std::string& username, const std::variant<std::string, std::chrono::system_clock::time_point>& date, IntervalType intervalType);

/**
 * Generate summaries for all active contributors in a repository for a specific time interval
 */

    // Query parameters for this interval

      // Check if summary already exists (skip if overwrite is true)

      // Get metrics for this contributor

      // Store summary in the database with interval type


} // namespace elizaos
