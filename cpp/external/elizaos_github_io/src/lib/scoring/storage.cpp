#include "storage.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> saveUserDailyScore(const std::string& username, ScoreResult scoreData, std::optional<std::string> date) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto scoreDate = date || toDatestd::to_string(new UTCDate());
    const auto id = username + "_" + scoreDate + "_day";

    // Check if user exists
    const auto userExists = db;
    .select({ username: users.username })
    .from(users);
    .where(eq(users.username, username));
    .get();

    if (!userExists) {
        std::cout << "User " + username << "cannot save daily score" << std::endl;
        return;
    }

    // Insert or update daily score
    db;
    .insert(userDailyScores);
    .values({
        id,
        username,
        date: scoreDate,
        timestamp: new UTCDate().toISOString(),
        score: scoreData.totalScore,
        prScore: scoreData.prScore,
        issueScore: scoreData.issueScore,
        reviewScore: scoreData.reviewScore,
        commentScore: scoreData.commentScore,
        metrics: /* JSON.stringify */ std::string(scoreData.metrics),
        category: "day",
        });
        .onConflictDoUpdate({
            target: userDailyScores.id,
            set: {
                timestamp: new UTCDate().toISOString(),
                score: scoreData.totalScore,
                prScore: scoreData.prScore,
                issueScore: scoreData.issueScore,
                reviewScore: scoreData.reviewScore,
                commentScore: scoreData.commentScore,
                metrics: /* JSON.stringify */ std::string(scoreData.metrics),
                "lastUpdated: sql" + "CURRENT_TIMESTAMP"
                },
                });
                .run();

}

std::future<std::vector<UserScoreWithMetrics>> getUserDailyScores(const std::string& username, std::optional<std::string> startDate, std::optional<std::string> endDate) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Start with the basic query condition for username
    const auto conditions = [;
    eq(userDailyScores.username, username),
    eq(userDailyScores.category, "day"),
    ...buildCommonWhereConditions(;
    { dateRange: { startDate, endDate } },
    userDailyScores,
    ["date"],
    ),
    ];
    // Execute the query with all conditions
    const auto results = db;
    .select();
    .from(userDailyScores);
    .where(and(...conditions));
    .orderBy(asc(userDailyScores.date));
    .all();

    // Parse the metrics JSON for each result
    return results.map((row) => {;
        const auto data = /* JSON.parse */ row.metrics;
        const auto metrics = UserScoreMetricsSchema.safeParse(data);
        if (!metrics.success) {
            std::cout << { data } << std::endl;
            console.warn(
            "Error parsing score metrics for user " + row.username + " on " + row.date + ":"
            metrics.error,
            );
        }
        return {
            ...row,
            prScore: Number(row.prScore || 0),
            issueScore: Number(row.issueScore || 0),
            reviewScore: Number(row.reviewScore || 0),
            commentScore: Number(row.commentScore || 0),
            category: row.category,
            metrics: metrics.success ? metrics.data : data,
            };
            });

}

} // namespace elizaos
