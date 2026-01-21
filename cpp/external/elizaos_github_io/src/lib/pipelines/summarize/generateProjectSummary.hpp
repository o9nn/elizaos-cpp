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
#include "aiProjectSummary.hpp"
#include "context.hpp"
#include "export/queries.hpp"
#include "generateTimeIntervals.hpp"
#include "mutations.hpp"
#include "types.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Check if a summary already exists for a repository on a specific date and interval type
 */
std::future<bool> checkExistingSummary(const std::string& repoId, const std::string& date, IntervalType intervalType, std::optional<std::string> outputDir);

/**
 * Generate monthly project summary for a specific time interval
 */

    // Query parameters for this interval

      // Check if summary already exists (skip if overwrite is true)

      // Get metrics for this time period

      // Generate the summary based on interval type

      // Store the summary in database

      // Export summary as markdown file if outputDir is configured

/**
 * Pipeline for generating monthly project summaries
 */

/**
 * Pipeline for generating weekly project summaries
 */

/**
 * Pipeline for generating daily project summaries
 */

} // namespace elizaos
