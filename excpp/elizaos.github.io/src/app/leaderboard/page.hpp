#include "queries.hpp"
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

default async ,
    { id: "monthly" as const, title: "Monthly", ...monthlyLeaderboard },
    { id: "weekly" as const, title: "Weekly", ...weeklyLeaderboard },
  ];
  const tags = await getAllTags();

  return (
    <main className="container mx-auto space-y-8 p-4">
      <Suspense fallback={<LeaderboardFallback />}>
        <Leaderboard tabs={tabs} tags={tags} />
      </Suspense>
    </main>
  );
}

} // namespace elizaos
