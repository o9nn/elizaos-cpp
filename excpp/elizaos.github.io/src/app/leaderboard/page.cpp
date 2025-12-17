#include "page.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> LeaderboardPage() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Fetch leaderboard data for all time periods using the new DB-based query
    const auto [allTimeLeaderboard, monthlyLeaderboard, weeklyLeaderboard] =;
    Promise.all([;
    getLeaderboard("all"),
    getLeaderboard("monthly"),
    getLeaderboard("weekly"),
    ]);

    const auto tabs = [;
    { id: "all", title: "All Time", ...allTimeLeaderboard },
    { id: "monthly", title: "Monthly", ...monthlyLeaderboard },
    { id: "weekly", title: "Weekly", ...weeklyLeaderboard },
    ];
    const auto tags = getAllTags();

    return (;
    <main className="container mx-auto space-y-8 p-4">;
    <Suspense fallback={<LeaderboardFallback />}>;
    <Leaderboard tabs={tabs} tags={tags} />;
    </Suspense>;
    </main>;
    );

}

} // namespace elizaos
