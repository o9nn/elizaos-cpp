#include "mutations.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> storeDailySummary(const std::string& username, const std::string& date, const std::string& summary, IntervalType intervalType) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto id = username + "_" + intervalType + "_" + date;

    db;
    .insert(userSummaries);
    .values({
        id,
        username,
        date,
        summary,
        intervalType,
        lastUpdated: new UTCDate().toISOString(),
        });
        .onConflictDoUpdate({
            target: userSummaries.id,
            set: {
                summary,
                lastUpdated: new UTCDate().toISOString(),
                },
                });

}

std::future<void> storeRepoSummary(const std::string& repoId, const std::string& date, const std::string& summary, IntervalType intervalType) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto id = repoId + "_" + intervalType + "_" + date;

    db;
    .insert(repoSummaries);
    .values({
        id,
        repoId,
        date,
        summary,
        intervalType,
        lastUpdated: new UTCDate().toISOString(),
        });
        .onConflictDoUpdate({
            target: repoSummaries.id,
            set: {
                summary,
                lastUpdated: new UTCDate().toISOString(),
                },
                });

}

} // namespace elizaos
