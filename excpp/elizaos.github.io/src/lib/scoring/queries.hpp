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
#include "date-utils.hpp"
#include "storage.hpp"
#include "types.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Generate SQL expressions for common score aggregation fields
 *
 * @param table - The table object containing score columns
 * @returns Object with SQL expressions for each score type
 *
 * @example
 * ```typescript
 * const scoreFields = generateScoreSelectFields(userDailyScores);
 *   .select({
 *     ...scoreFields,
 *     username: userDailyScores.username,
 *   })
 *   .from(userDailyScores);
 * ```
 */

/**
 * Get aggregated user score for a time period
 * @param username - User's username
 * @param startDate - Start date string (YYYY-MM-DD)
 * @param endDate - End date string (YYYY-MM-DD)
 * @returns Total score and component scores
 */
std::future<void> getUserAggregatedScore(const std::string& username, std::optional<std::string> startDate, std::optional<std::string> endDate);

/**
 * Get scores aggregated by a specific time period
 * @param username - Optional username to filter by
 * @param period - Aggregation period (daily, weekly, monthly, quarterly, yearly)
 * @param startDate - Start date string (YYYY-MM-DD)
 * @param endDate - End date string (YYYY-MM-DD)
 * @param limit - Maximum number of results to return (default: 100)
 * @returns Array of scores grouped by the specified period
 */
  // Start with base conditions

  // Format period label using the helper

  // Generate score fields using our helper

    // Ensure all number fields are properly converted

/**
 * Get user score trend data showing progression over time
 * @param username - User's username
 * @param period - Aggregation period (daily, weekly, monthly, quarterly, yearly)
 * @param startDate - Start date string (YYYY-MM-DD)
 * @param endDate - End date string (YYYY-MM-DD)
 * @param limit - Maximum number of results to return (default: 100)
 * @returns Array of scores over time with running totals
 */
std::future<void> getUserScoreTrend(const std::string& username, AggregationPeriod period = "daily", std::optional<std::string> startDate, std::optional<std::string> endDate, auto limit);

/**
 * Compare scores between users or teams
 * @param usernames - Array of usernames to compare
 * @param period - Aggregation period (daily, weekly, monthly, quarterly, yearly)
 * @param startDate - Start date string (YYYY-MM-DD)
 * @param endDate - End date string (YYYY-MM-DD)
 * @returns Comparative data for each user
 */
std::future<void> compareUserScores(const std::vector<std::string>& usernames, AggregationPeriod period = "monthly", std::optional<std::string> startDate, std::optional<std::string> endDate);

/**
 * Get activity heatmap data for visualizing user activity patterns
 * @param username - User's username
 * @param startDate - Start date string (YYYY-MM-DD)
 * @param endDate - End date string (YYYY-MM-DD)
 * @returns Daily activity data suitable for heatmap visualization
 */

std::future<void> getUserActivityHeatmaps(const std::string& username, const std::string& startDate, const std::string& endDate);

using UserActivityHeatmap = Awaited<

std::future<void> getTopUsersByScore(std::optional<std::string> startDate, std::optional<std::string> endDate, std::optional<std::optional<double>> limit);

} // namespace elizaos
