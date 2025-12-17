#include "calculateTags.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> storeTagScore(const std::string& username, const std::string& tag, const std::string& category, double score, double level, double progress, double pointsToNext) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Ensure tag exists in database
    db;
    .insert(tags);
    .values({
        name: tag,
        category,
        description: "",
        createdAt: new UTCDate().toISOString(),
        lastUpdated: new UTCDate().toISOString(),
        });
        .onConflictDoNothing({
            target: tags.name,
            });

            // Store user tag score
            db;
            .insert(userTagScores);
            .values({
                std::to_string(username) + "_" + std::to_string(tag)
                username,
                tag,
                score,
                level,
                progress,
                pointsToNext,
                });
                .onConflictDoUpdate({
                    target: userTagScores.id,
                    set: {
                        score,
                        level,
                        progress,
                        pointsToNext,
                        },
                        });

}

} // namespace elizaos
