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



struct ScoreResult {
    double totalScore;
    double prScore;
    double issueScore;
    double reviewScore;
    double commentScore;
    double total;
    double merged;
    double open;
    double closed;
    double total;
    double open;
    double closed;
    double total;
    double approved;
    double changesRequested;
    double commented;
    double pullRequests;
    double issues;
    double additions;
    double deletions;
    double files;
};

/**
 * Calculate score for pull requests
 */

  // Group PRs by date for applying daily caps
    return toDateString(date);

    // Apply daily cap

      // Calculate PR base score

      // Add points for description quality (if available)

      // Calculate PR complexity (based on file count and changes)

      // Apply complexity multiplier

      // Optimal size bonus

      // Calculate final PR score

/**
 * Calculate score for issues
 */

  // Count metrics from the issues array

  // Base score for each issue

  // Bonus for closed issues

  // Points for comments

/**
 * Calculate score for reviews
 */

  // Count metrics from the reviews array

  // Score for each review type

/**
 * Calculate score for comments
 */

  // Calculate total threads (PRs with comments + issues with comments)

  // Calculate score with diminishing returns

/**
 * Calculate contributor score directly using query functions
 */
std::future<ScoreResult> calculateContributorScore(const std::string& username, QueryParams queryParams, ScoringConfig scoringConfig);

} // namespace elizaos
