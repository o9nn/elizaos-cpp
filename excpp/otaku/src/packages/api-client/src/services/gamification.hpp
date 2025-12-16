#include ".lib/base-client.hpp"
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

;
;

struct LeaderboardEntry {
    double rank;
    std::string userId;
    double points;
    double level;
    std::string levelName;
    std::optional<std::string> username;
    std::optional<std::string> avatar;
};


struct LeaderboardResponse {
    'weekly' | 'all_time' scope;
    std::vector<LeaderboardEntry> entries;
    double userRank;
    double limit;
};


struct UserSummary {
    std::string userId;
    double allTimePoints;
    double weeklyPoints;
    double streakDays;
    double level;
    std::string levelName;
    std::optional<{> nextMilestone;
    double level;
    std::string levelName;
    double pointsNeeded;
    string | null lastLoginDate;
    std::optional<double> swapsCompleted;
};


struct ReferralStats {
    double totalReferrals;
    double activatedReferrals;
    double totalPointsEarned;
};


struct ReferralCodeResponse {
    std::string code;
    ReferralStats stats;
    std::string referralLink;
};


class GamificationService extends BaseApiClient {
  /**
   * Get leaderboard data
   * @param agentId Agent ID to route the request to
   * @param scope Leaderboard scope ('weekly' or 'all_time')
   * @param limit Number of entries to return (default: 50)
   * @param userId Optional user ID to get user's rank
   */
  async getLeaderboard(
    agentId: UUID,
    scope: 'weekly' | 'all_time' = 'weekly',
    limit: number = 50,
    userId?: UUID
  ): Promise<LeaderboardResponse> {
    const params: Record<string, string> = {
      scope,
      limit: limit.toString(),
    };
    
    if (userId) {
      params.userId = userId;
    }

    return this.get<LeaderboardResponse>(
      `/api/agents/${agentId}/plugins/gamification/leaderboard`,
      { params }
    );
  }

  /**
   * Get user summary with points, level, streak, and swap count
   * @param agentId Agent ID to route the request to
   * @param userId User ID to get summary for
   */
  async getUserSummary(
    agentId: UUID,
    userId: UUID
  ): Promise<UserSummary> {
    return this.get<UserSummary>(
      `/api/agents/${agentId}/plugins/gamification/summary`,
      { params: { userId } }
    );
  }

  /**
   * Get or create referral code for user
   * @param agentId Agent ID to route the request to
   * @param userId User ID to get referral code for
   */
  async getReferralCode(
    agentId: UUID,
    userId: UUID
  ): Promise<ReferralCodeResponse> {
    return this.get<ReferralCodeResponse>(
      `/api/agents/${agentId}/plugins/gamification/referral`,
      { params: { userId } }
    );
  }
}


} // namespace elizaos
