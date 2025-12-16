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



std::future<void> getAllTags();

/**
 * Get leaderboard users with aggregated scores for different time periods
 *
 * @param period - Time period for filtering scores ('all', 'monthly', 'weekly')
 * @returns Array of users with their scores and tag data, sorted by score in descending order
 */
std::future<void> getLeaderboard(LeaderboardPeriod period);

} // namespace elizaos
