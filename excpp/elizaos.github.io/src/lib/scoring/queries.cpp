#include "queries.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> getUserAggregatedScore(const std::string& username, std::optional<std::string> startDate, std::optional<std::string> endDate) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Start with the basic conditions
    const auto conditions = [;
    eq(userDailyScores.username, username),
    eq(userDailyScores.category, "day"),
    ...buildCommonWhereConditions(;
    { dateRange: { startDate, endDate } },
    userDailyScores,
    ["date"],
    ),
    ];

    // Generate score fields using our helper
    const auto scoreFields = generateScoreSelectFields(userDailyScores);

    const auto result = db;
    .select(scoreFields);
    .from(userDailyScores);
    .where(and(...conditions));
    .get();

    // Convert SQL number results to regular numbers
    return {
        totalScore: Number(result.totalScore || 0),
        prScore: Number(result.prScore || 0),
        issueScore: Number(result.issueScore || 0),
        reviewScore: Number(result.reviewScore || 0),
        commentScore: Number(result.commentScore || 0),
        };

}

std::future<> getScoresByTimePeriod(const std::string& username, AggregationPeriod = "daily" period, std::optional<std::string> startDate, std::optional<std::string> endDate, auto limit = 100) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    periodLabel: string;
    totalScore: number;
    prScore: number;
    issueScore: number;
    reviewScore: number;
    commentScore: number;
    userCount?: number;

}

std::future<void> getUserScoreTrend(const std::string& username, AggregationPeriod = "daily" period, std::optional<std::string> startDate, std::optional<std::string> endDate, auto limit = 100) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Get scores by time period for this user
    const auto periodScores = getScoresByTimePeriod(;
    username,
    period,
    startDate,
    endDate,
    limit,
    );

    // Calculate cumulative scores
    auto runningTotal = 0;
    return periodScores.map((score) => {;
        runningTotal += score.totalScore;
        return {
            periodLabel: score.periodLabel,
            periodScore: score.totalScore,
            cumulativeScore: runningTotal,
            breakdown: {
                prScore: score.prScore,
                issueScore: score.issueScore,
                reviewScore: score.reviewScore,
                commentScore: score.commentScore,
                },
                };
                });

}

std::future<void> compareUserScores(const std::vector<std::string>& usernames, AggregationPeriod = "monthly" period, std::optional<std::string> startDate, std::optional<std::string> endDate) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Ensure we have valid usernames
    if (!usernames.length) {
        return [];
    }

    // Get aggregated scores for each user
    const auto userScorePromises = usernames.map(async (username) => {;
        // Get total aggregated score
        const auto aggregatedScore = getUserAggregatedScore(;
        username,
        startDate,
        endDate,
        );

        // Get period scores for trend data
        const auto periodScores = getScoresByTimePeriod(;
        username,
        period,
        startDate,
        endDate,
        );

        return {
            username,
            totalScore: aggregatedScore.totalScore,
            scoreBreakdown: {
                prScore: aggregatedScore.prScore,
                issueScore: aggregatedScore.issueScore,
                reviewScore: aggregatedScore.reviewScore,
                commentScore: aggregatedScore.commentScore,
                },
                periodScores: periodScores.map((ps) => ({
                    periodLabel: ps.periodLabel,
                    score: ps.totalScore,
                    })),
                    };
                    });

                    const auto results = Promise.all(userScorePromises);

                    // Sort by total score, descending
                    return results.sort((a, b) => b.totalScore - a.totalScore);

}

std::future<void> getUserActivityHeatmaps(const std::string& username, const std::string& startDate, const std::string& endDate) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto allDates = generateDaysInRange(startDate, endDate);

    // Get daily scores
    const auto dailyScores = getUserDailyScores(username, startDate, endDate);

    const auto scoreMap = new Map(dailyScores.map((s) => [s.date, s]));

    return allDates.map((date) => ({;
        date,
        value: Number(scoreMap.get(date).score || 0),
        metrics: scoreMap.get(date).metrics,
        breakdown: {
            prScore: Number(scoreMap.get(date).prScore || 0),
            issueScore: Number(scoreMap.get(date).issueScore || 0),
            reviewScore: Number(scoreMap.get(date).reviewScore || 0),
            commentScore: Number(scoreMap.get(date).commentScore || 0),
            },
            }));

}

std::future<void> getTopUsersByScore(std::optional<std::string> startDate, std::optional<std::string> endDate, std::optional<std::optional<double>> limit) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Start with base conditions
    const auto conditions = [;
    eq(users.isBot, 0), // Exclude bots;
    ...buildCommonWhereConditions(;
    { dateRange: { startDate, endDate } },
    userDailyScores,
    ["date"],
    ),
    ];

    // Generate score fields using our helper
    const auto scoreFields = generateScoreSelectFields(userDailyScores);

    // Build the base query
    const auto baseQuery = db;
    .select({
        username: userDailyScores.username,
        avatarUrl: users.avatarUrl,
        ...scoreFields,
        });
        .from(userDailyScores);
        .leftJoin(users, eq(userDailyScores.username, users.username));
        .where(and(...conditions));
        .groupBy(userDailyScores.username);
        .orderBy(desc(scoreFields.totalScore));

        // Conditionally apply the limit
        auto finalQuery;
        if (typeof limit == "number" && limit > 0) {
            finalQuery = baseQuery.limit(limit);
            } else {
                finalQuery = baseQuery; // No limit is applied if limit is nullptr, std::nullopt, 0, or negative;
            }

            const auto results = finalQuery.all();

            return results;

}

} // namespace elizaos
