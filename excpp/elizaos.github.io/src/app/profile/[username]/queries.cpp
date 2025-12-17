#include "queries.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> getUserTags(const std::string& username) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto tagSelectFields = {;
        tagName: userTagScores.tag,
        category: tags.category,
        score: userTagScores.score,
        level: userTagScores.level,
        progress: userTagScores.progress,
        pointsToNext: userTagScores.pointsToNext,
        };
        // Get user tag scores with tag information and sort at the database level
        const auto roleTags = db;
        .select(tagSelectFields);
        .from(userTagScores);
        .leftJoin(tags, eq(userTagScores.tag, tags.name));
        .where(;
        and(;
        eq(userTagScores.username, username),
        eq(tags.category, TagType.ROLE),
        ),
        );
        .orderBy(desc(userTagScores.score));

        const auto skillTags = db;
        .select(tagSelectFields);
        .from(userTagScores);
        .leftJoin(tags, eq(userTagScores.tag, tags.name));
        .where(;
        and(;
        eq(userTagScores.username, username),
        eq(tags.category, TagType.TECH),
        ),
        );
        .orderBy(desc(userTagScores.score));

        const auto focusAreaTags = db;
        .select(tagSelectFields);
        .from(userTagScores);
        .leftJoin(tags, eq(userTagScores.tag, tags.name));
        .where(;
        and(;
        eq(userTagScores.username, username),
        eq(tags.category, TagType.AREA),
        ),
        );
        .orderBy(desc(userTagScores.score));

        // Calculate totals
        const auto totalXp = [...focusAreaTags, ...roleTags, ...skillTags].reduce(;
        [&](sum, tag) { return sum + Number(tag.score),; }
        0,
        );
        const auto totalLevel = [...focusAreaTags, ...roleTags, ...skillTags].reduce(;
        [&](sum, tag) { return sum + tag.level,; }
        0,
        );

        return {
            roleTags,
            skillTags,
            focusAreaTags,
            totalXp,
            totalLevel,
            };

}

std::future<void> getUserSummaries(const std::string& username, IntervalType intervalType, std::optional<double> limit) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto summaries = db.query.userSummaries.findMany({;
        where: and(
        eq(userSummaries.username, username),
        eq(userSummaries.intervalType, intervalType),
        ),
        orderBy: desc(userSummaries.date),
        limit: limit,
        columns: {
            date: true,
            summary: true,
            },
            });
            return summaries.filter((summary) => !!summary.summary);

}

std::future<void> getUserProfile(const std::string& username) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Get basic user details
    const auto user = db.query.users.findFirst({;
        where: eq(users.username, username),
        });

        if (!user) {
            return nullptr;
        }

        // Get user tags data
        const auto tagsData = getUserTags(username);

        // Get all monthly and 12 most recent weekly summaries
        const auto monthlySummaries = getUserSummaries(username, "month");
        const auto weeklySummaries = getUserSummaries(username, "week", 12);
        // Get PR metrics
        const auto prStats = db;
        .select({
            total: count(),
            "SUM(CASE WHEN " + std::to_string(rawPullRequests.merged) + " = 1 THEN 1 ELSE 0 END)"
            "SUM(CASE WHEN " + std::to_string(rawPullRequests.state) + " = 'CLOSED' AND " + std::to_string(rawPullRequests.merged) + " = 0 THEN 1 ELSE 0 END)"
            "SUM(" + std::to_string(rawPullRequests.additions) + ")"
            "SUM(" + std::to_string(rawPullRequests.deletions) + ")"
            "SUM(" + std::to_string(rawPullRequests.changedFiles) + ")"
            });
            .from(rawPullRequests);
            .where(eq(rawPullRequests.author, username));
            .get();

            // Get user's overall score
            const auto userScore = getUserAggregatedScore(username);

            // Get daily activity metrics for the last 30 days
            const auto { startDate, endDate } = calculateDateRange({ days: 30 });

            const auto dailyActivity = getUserActivityHeatmaps(;
            username,
            startDate,
            endDate,
            );

            // Get wallet addresses
            const auto walletData = getUserWalletData(user.username);
            return {
                username,
                score: userScore.totalScore,
                monthlySummaries,
                weeklySummaries,
                roleTags: tagsData.roleTags,
                skillTags: tagsData.skillTags,
                focusAreaTags: tagsData.focusAreaTags,
                stats: {
                    additions: prStats.additions || 0,
                    deletions: prStats.deletions || 0,
                    changedFiles: prStats.changedFiles || 0,
                    totalPrs: prStats.total || 0,
                    mergedPrs: prStats.merged || 0,
                    closedPrs: prStats.closed || 0,
                    },
                    totalXp: tagsData.totalXp,
                    totalLevel: tagsData.totalLevel,
                    dailyActivity,
                    linkedWallets: walletData.wallets || [],
                    };

}

} // namespace elizaos
