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



/**
 * Time period options for aggregation queries
 */
using AggregationPeriod = std::variant<, "daily", "weekly", "monthly", "quarterly", "yearly">;

using UserScoreMetrics = z.infer<typeof UserScoreMetricsSchema>;
/**
 * Interface for userDailyScores with typed metrics
 */

using UserScoreWithMetrics = InferSelectModel<typeof userDailyScores> & {

enum TagType {
  AREA = "AREA",
  ROLE = "ROLE",
  TECH = "TECH",
}

} // namespace elizaos
