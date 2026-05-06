#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".date-utils.hpp"
#include "scoreCalculator.hpp"
#include "types.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Save daily score for a user
 * @param username - User's username
 * @param scoreData - Score data from scoring.ts
 * @param date - Optional date string (YYYY-MM-DD) - defaults to today
 */
std::future<void> saveUserDailyScore(const std::string& username, ScoreResult scoreData, std::optional<std::string> date);

/**
 * Get daily scores for a user
 * @param username - User's username
 * @param startDate - Start date string (YYYY-MM-DD)
 * @param endDate - End date string (YYYY-MM-DD)
 * @param category - Optional category filter
 * @returns Array of daily score records with parsed metrics
 */

} // namespace elizaos
