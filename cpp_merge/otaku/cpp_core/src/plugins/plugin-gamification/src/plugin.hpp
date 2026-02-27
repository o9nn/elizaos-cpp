#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "actions/getLeaderboard.hpp"
#include "actions/getPointsSummary.hpp"
#include "actions/getReferralCode.hpp"
#include "elizaos/core.hpp"
#include "events/eventHandlers.hpp"
#include "providers/leaderboardProvider.hpp"
#include "providers/pointsProvider.hpp"
#include "schema.hpp"
#include "services/GamificationService.hpp"
#include "services/LeaderboardService.hpp"
#include "services/ReferralService.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Leaderboard route handlers
std::future<void> handleGetLeaderboard(const std::string& req, const std::string& res, IAgentRuntime runtime);

// User summary route handler
std::future<void> handleGetUserSummary(const std::string& req, const std::string& res, IAgentRuntime runtime);

// Referral code route handler
std::future<void> handleGetReferralCode(const std::string& req, const std::string& res, IAgentRuntime runtime);

  // Note: Plugin routes are automatically protected by the general API rate limit middleware
  // (1000 requests per 15 minutes per IP) applied in createApiRouter.
  // Additional input validation is performed in the handlers themselves.


} // namespace elizaos
