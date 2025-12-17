#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "callAIService.hpp"
#include "config.hpp"
#include "queries.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



using ContributorMetricsForSummary = Awaited<

/**
 * Generate an AI summary of a contributor's activity
 */

/**
 * Format contributor metrics into a structured prompt
 */
std::string formatContributorPrompt(ContributorMetricsForSummary metrics, IntervalType intervalType);

} // namespace elizaos
