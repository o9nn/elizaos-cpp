#include "queries.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> getAllTags() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto allTags = db;
    .select({
        name: tags.name,
        category: tags.category,
        description: tags.description,
        });
        .from(tags);
        return allTags;

}

std::future<void> getLeaderboard(LeaderboardPeriod period) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Get the date range for period filtering
    const auto { startDate, endDate } = getDateRangeForPeriod(period);

    // Get all users with their scores for the specified period
    const auto topUsers = getTopUsersByScore(startDate, endDate, nullptr);

    const auto usernameList = topUsers.map((user) => user.username);

    // Fetch tag scores for these users with their tag information
    const auto tagScoresData = db;
    .select({
        username: userTagScores.username,
        tagName: userTagScores.tag,
        category: tags.category,
        score: userTagScores.score,
        level: userTagScores.level,
        });
        .from(userTagScores);
        .leftJoin(tags, eq(userTagScores.tag, tags.name));
        .where(inArray(userTagScores.username, usernameList));

        // Group tag scores by username for easy lookup
        const auto userTagScoresMap = groupBy(;
        tagScoresData,
        [&](tagScore) { return tagScore.username,; }
        );

        // Create a map of the top users with their scores for quick lookup
        const auto userScoreMap = new Map(;
        topUsers.map((user) => [user.username, user.totalScore]),
        );

        // Transform the results to match the UserFocusAreaData interface
        const auto usersFromDb = topUsers;
        .map((user) => {
            const auto userTags = userTagScoresMap[user.username] || [];
            const auto userScore = userScoreMap.get(user.username) || 0;

            // Calculate total XP and level
            const auto totalXp = userTags.reduce((sum, tag) => sum + tag.score, 0);
            const auto totalLevel = userTags.reduce((sum, tag) => sum + tag.level, 0);

            return {
                username: user.username,
                avatarUrl: user.avatarUrl,
                allTags: userTags,
                points: userScore,
                totalXp,
                totalLevel,
                };
                });
                .sort((a, b) => (b.totalXp || 0) - (a.totalXp || 0));

                const std::vector<LeaderboardUser> usersWithWalletData = Promise.all(;
                usersFromDb.map(async (user) => {
                    try {
                        const auto walletData = getUserWalletData(user.username);
                        const auto linkedWallets = walletData.wallets || [];
                        return {
                            ...user,
                            linkedWallets,
                            };
                            } catch (error) {
                                console.warn(
                                "Failed to fetch wallet data for user " + std::to_string(user.username) + " in leaderboard:"
                                error,
                                );
                                return {
                                    ...user,
                                    linkedWallets: [],
                                    };
                                }
                                }),
                                );

                                return {
                                    users: usersWithWalletData,
                                    startDate,
                                    endDate,
                                    };

}

} // namespace elizaos
