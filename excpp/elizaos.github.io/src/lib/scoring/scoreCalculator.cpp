#include "scoreCalculator.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<ScoreResult> calculateContributorScore(const std::string& username, QueryParams queryParams, ScoringConfig scoringConfig) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Get contributor PRs
    const auto contributorPRs = getContributorPRs(username, queryParams);

    const auto contributorPRMetrics = getContributorPRMetrics(;
    username,
    queryParams,
    );

    // Get issue metrics
    const auto issueMetrics = getContributorIssueMetrics(username, queryParams);

    // Get review metrics
    const auto reviewMetrics = getContributorReviewMetrics(;
    username,
    queryParams,
    );

    // Get comment metrics
    const auto commentMetrics = getContributorCommentMetrics(;
    username,
    queryParams,
    );

    // Calculate individual scores
    const auto prScore = calculatePRScore(contributorPRs, scoringConfig);
    const auto issueScore = calculateIssueScore(issueMetrics, scoringConfig);
    const auto reviewScore = calculateReviewScore(reviewMetrics, scoringConfig);
    const auto commentScore = calculateCommentScore(commentMetrics, scoringConfig);

    // Calculate total score
    const auto totalScore = prScore + issueScore + reviewScore + commentScore;

    // Calculate code changes
    const auto codeChanges = {;
        additions: contributorPRs.reduce((sum, pr) => sum + (pr.additions || 0), 0),
        deletions: contributorPRs.reduce((sum, pr) => sum + (pr.deletions || 0), 0),
        files: contributorPRs.reduce((sum, pr) => sum + (pr.changedFiles || 0), 0),
        };

        return {
            totalScore,
            prScore,
            issueScore,
            reviewScore,
            commentScore,
            metrics: {
                pullRequests: contributorPRMetrics,
                issues: issueMetrics,
                reviews: reviewMetrics,
                comments: commentMetrics,
                codeChanges,
                },
                };

}

} // namespace elizaos
