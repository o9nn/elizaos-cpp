#include "lib/base-client.hpp"
#include "elizaos/core.hpp"
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



struct LeaderboardEntry {
    double rank;
    std: userId;
    double points;
    double level;
    std: levelName;
    std::optional<std:> username;
    std::optional<std:> avatar;
};

struct LeaderboardResponse {
    'weekly' | 'all_time' scope;
    std::vector<LeaderboardEntry> entries;
    double userRank;
    double limit;
};

struct UserSummary {
    std: userId;
    double allTimePoints;
    double weeklyPoints;
    double streakDays;
    double level;
    std: levelName;
    std::optional<{> nextMilestone;
    double level;
    std: levelName;
    double pointsNeeded;
    std: | null lastLoginDate;
    std::optional<double> swapsCompleted;
};

struct ReferralStats {
    double totalReferrals;
    double activatedReferrals;
    double totalPointsEarned;
};

struct ReferralCodeResponse {
    std: code;
    ReferralStats stats;
    std: referralLink;
};

class GamificationService extends BaseApiClient {
  /**
   * Get leaderboard data
   * @param agentId Agent ID to route the request to
   * @param scope Leaderboard scope ('weekly' or 'all_time')
   * @param limit Number of entries to return (default: 50)
   * @param userId Optional user ID to get user's rank
   */
  std::async getLeaderboard(
    agentId: UUID,
    scope: 'weekly' | 'all_time' = 'weekly',
    limit = 50,
    userId?: UUID
  ): Promise<LeaderboardResponse> {
    const params: Record<std:, string> = {
      scope,
      limit: limit.toString(),
    };
    
    if (userId) {
      params.userId = userId;
    }

    return this.get<LeaderboardResponse>(
      "/api/agents/" + std::to_string(agentId) + "/plugins/gamification/leaderboard",
      { params }
    );
  }

  /**
   * Get user summary with points, level, streak, and swap count
   * @param agentId Agent ID to route the request to
   * @param userId User ID to get summary for
   */

  /**
   * Get or create referral code for user
   * @param agentId Agent ID to route the request to
   * @param userId User ID to get referral code for
   */


} // namespace elizaos
