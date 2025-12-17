#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Time period options for aggregation queries
 */
using AggregationPeriod = ;

using UserScoreMetrics = z::infer<typeof UserScoreMetricsSchema>;
/**
 * Interface for userDailyScores with typed metrics
 */

using UserScoreWithMetrics = InferSelectModel<typeof userDailyScores>;

enum TagType {
}

} // namespace elizaos
