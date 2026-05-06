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
#include "elizaos/core.hpp"
#include "lib/base-client.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



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
    std::string scope;
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
    double level;
    std::string levelName;
    double pointsNeeded;
    std::string lastLoginDate;
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

class GamificationService {
public:
    std::future<UserSummary> getUserSummary(UUID agentId, UUID userId);
    std::future<ReferralCodeResponse> getReferralCode(UUID agentId, UUID userId);
};


} // namespace elizaos
