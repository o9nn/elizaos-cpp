#include ".date-utils.hpp"
#include "scoreCalculator.hpp"
#include "types.hpp"
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

;
;
;
;
;
;
;
;
;

/**
 * Save daily score for a user
 * @param username - User's username
 * @param scoreData - Score data from scoring.ts
 * @param date - Optional date string (YYYY-MM-DD) - defaults to today
d * @returns Result of the insert operation
 */
std::future<void> saveUserDailyScore(const std::string& username, ScoreResult scoreData, std::optional<std::string> date);_${scoreDate}_day`;

  // Check if user exists
  const userExists = await db
    .select({ username: users.username })
    .from(users)
    .where(eq(users.username, username))
    .get();

  if (!userExists) {
    console.warn(`User ${username} not found, cannot save daily score`);
    return;
  }

  // Insert or update daily score
  await db
    .insert(userDailyScores)
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
      metrics: JSON.stringify(scoreData.metrics),
      category: "day",
    })
    .onConflictDoUpdate({
      target: userDailyScores.id,
      set: {
        timestamp: new UTCDate().toISOString(),
        score: scoreData.totalScore,
        prScore: scoreData.prScore,
        issueScore: scoreData.issueScore,
        reviewScore: scoreData.reviewScore,
        commentScore: scoreData.commentScore,
        metrics: JSON.stringify(scoreData.metrics),
        lastUpdated: sql`CURRENT_TIMESTAMP`,
      },
    })
    .run();
}

/**
 * Get daily scores for a user
 * @param username - User's username
 * @param startDate - Start date string (YYYY-MM-DD)
 * @param endDate - End date string (YYYY-MM-DD)
 * @param category - Optional category filter
 * @returns Array of daily score records with parsed metrics
 */
std::future<std::vector<UserScoreWithMetrics>> getUserDailyScores(const std::string& username, std::optional<std::string> startDate, std::optional<std::string> endDate); },
      userDailyScores,
      ["date"],
    ),
  ];
  // Execute the query with all conditions
  const results = await db
    .select()
    .from(userDailyScores)
    .where(and(...conditions))
    .orderBy(asc(userDailyScores.date))
    .all();

  // Parse the metrics JSON for each result
  return results.map((row) => {
    const data = JSON.parse(row.metrics);
    const metrics = UserScoreMetricsSchema.safeParse(data);
    if (!metrics.success) {
      console.log({ data });
      console.warn(
        `Error parsing score metrics for user ${row.username} on ${row.date}:`,
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
