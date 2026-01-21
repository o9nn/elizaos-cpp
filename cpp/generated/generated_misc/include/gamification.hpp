#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTAKU_SRC_PACKAGES_API-CLIENT_SRC_SERVICES_GAMIFICATION_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTAKU_SRC_PACKAGES_API-CLIENT_SRC_SERVICES_GAMIFICATION_H
#include "core.h"
#include "@elizaos/core.h"
#include "../lib/base-client.h"

class LeaderboardEntry;
class LeaderboardResponse;
class UserSummary;
class ReferralStats;
class ReferralCodeResponse;
class GamificationService;

class LeaderboardEntry : public object, public std::enable_shared_from_this<LeaderboardEntry> {
public:
    using std::enable_shared_from_this<LeaderboardEntry>::shared_from_this;
    double rank;

    string userId;

    double points;

    double level;

    string levelName;

    string username;

    string avatar;
};

class LeaderboardResponse : public object, public std::enable_shared_from_this<LeaderboardResponse> {
public:
    using std::enable_shared_from_this<LeaderboardResponse>::shared_from_this;
    any scope;

    array<std::shared_ptr<LeaderboardEntry>> entries;

    double userRank;

    double limit;
};

class UserSummary : public object, public std::enable_shared_from_this<UserSummary> {
public:
    using std::enable_shared_from_this<UserSummary>::shared_from_this;
    string userId;

    double allTimePoints;

    double weeklyPoints;

    double streakDays;

    double level;

    string levelName;

    object nextMilestone;

    any lastLoginDate;

    double swapsCompleted;
};

class ReferralStats : public object, public std::enable_shared_from_this<ReferralStats> {
public:
    using std::enable_shared_from_this<ReferralStats>::shared_from_this;
    double totalReferrals;

    double activatedReferrals;

    double totalPointsEarned;
};

class ReferralCodeResponse : public object, public std::enable_shared_from_this<ReferralCodeResponse> {
public:
    using std::enable_shared_from_this<ReferralCodeResponse>::shared_from_this;
    string code;

    std::shared_ptr<ReferralStats> stats;

    string referralLink;
};

class GamificationService : public BaseApiClient, public std::enable_shared_from_this<GamificationService> {
public:
    using std::enable_shared_from_this<GamificationService>::shared_from_this;
    template <typename P1>
    std::shared_ptr<Promise<std::shared_ptr<LeaderboardResponse>>> getLeaderboard(std::shared_ptr<UUID> agentId, P1 scope = std::string("weekly"), double limit = 50, std::shared_ptr<UUID> userId = undefined);
    virtual std::shared_ptr<Promise<std::shared_ptr<UserSummary>>> getUserSummary(std::shared_ptr<UUID> agentId, std::shared_ptr<UUID> userId);
    virtual std::shared_ptr<Promise<std::shared_ptr<ReferralCodeResponse>>> getReferralCode(std::shared_ptr<UUID> agentId, std::shared_ptr<UUID> userId);
    GamificationService(std::shared_ptr<ApiClientConfig> config);
};

template <typename P1>
std::shared_ptr<Promise<std::shared_ptr<LeaderboardResponse>>> GamificationService::getLeaderboard(std::shared_ptr<UUID> agentId, P1 scope, double limit, std::shared_ptr<UUID> userId)
{
    auto params = object{
        object::pair{std::string("scope"), std::string("scope")}, 
        object::pair{std::string("limit"), limit->toString()}
    };
    if (userId) {
        params->userId = userId;
    }
    return this->get<std::shared_ptr<LeaderboardResponse>>(std::string("/api/agents/") + agentId + std::string("/plugins/gamification/leaderboard"), object{
        object::pair{std::string("params"), std::string("params")}
    });
}

#endif
