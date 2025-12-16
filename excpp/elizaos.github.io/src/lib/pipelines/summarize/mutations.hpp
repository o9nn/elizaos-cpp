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

/**
 * Store daily summary in the database
 */
std::future<void> storeDailySummary(const std::string& username, const std::string& date, const std::string& summary, IntervalType intervalType);_${intervalType}_${date}`;

  await db
    .insert(userSummaries)
    .values({
      id,
      username,
      date,
      summary,
      intervalType,
      lastUpdated: new UTCDate().toISOString(),
    })
    .onConflictDoUpdate({
      target: userSummaries.id,
      set: {
        summary,
        lastUpdated: new UTCDate().toISOString(),
      },
    });
}

/**
 * Store repository summary in the database
 */
std::future<void> storeRepoSummary(const std::string& repoId, const std::string& date, const std::string& summary, IntervalType intervalType);_${intervalType}_${date}`;

  await db
    .insert(repoSummaries)
    .values({
      id,
      repoId,
      date,
      summary,
      intervalType,
      lastUpdated: new UTCDate().toISOString(),
    })
    .onConflictDoUpdate({
      target: repoSummaries.id,
      set: {
        summary,
        lastUpdated: new UTCDate().toISOString(),
      },
    });
} // --- Helper functions ---

} // namespace elizaos
