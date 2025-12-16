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

/**
 * Time period options for aggregation queries
 */
using AggregationPeriod = std::variant<, "daily", "weekly", "monthly", "quarterly", "yearly">;

const UserScoreMetricsSchema = z.object({
  pullRequests: z.object({
    total: z.number(),
    merged: z.number(),
    open: z.number(),
    closed: z.number(),
  }),
  issues: z.object({
    total: z.number(),
    open: z.number(),
    closed: z.number(),
  }),
  reviews: z.object({
    total: z.number(),
    approved: z.number(),
    changesRequested: z.number(),
    commented: z.number(),
  }),
  comments: z.object({
    pullRequests: z.number(),
    issues: z.number(),
  }),
  codeChanges: z.object({
    additions: z.number(),
    deletions: z.number(),
    files: z.number(),
  }),
});

using UserScoreMetrics = z.infer<typeof UserScoreMetricsSchema>;
/**
 * Interface for userDailyScores with typed metrics
 */

using UserScoreWithMetrics = InferSelectModel<typeof userDailyScores> & {
  metrics: UserScoreMetrics;
};

enum TagType {
  AREA = "AREA",
  ROLE = "ROLE",
  TECH = "TECH",
}

} // namespace elizaos
