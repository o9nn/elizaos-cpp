#include "mutations.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> storeDailySummary(const std::string& username, const std::string& date, const std::string& summary, IntervalType intervalType) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto id = std::to_string(username) + "_" + std::to_string(intervalType) + "_" + std::to_string(date);

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

    const auto id = std::to_string(repoId) + "_" + std::to_string(intervalType) + "_" + std::to_string(date);

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
