#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_OTAKU_SRC_PLUGINS_PLUGIN_GAMIFICATION_SRC_TYPES_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_OTAKU_SRC_PLUGINS_PLUGIN_GAMIFICATION_SRC_TYPES_H
#include "core.hpp"
// External dependency removed
#include "./constants.h"

class GamificationEventInput;
class PointBalance;
class UserSummary;
class LeaderboardEntry;
class ReferralCode;
class ReferralStats;
class Campaign;

class GamificationEventInput : public object, public std::enable_shared_from_this<GamificationEventInput> {
public:
    using std::enable_shared_from_this<GamificationEventInput>::shared_from_this;
    std::shared_ptr<UUID> userId;

    GamificationEventType actionType;

    Record<string, any> metadata;

    std::shared_ptr<UUID> sourceEventId;

    double volumeUsd;

    string chain;
};

class PointBalance : public object, public std::enable_shared_from_this<PointBalance> {
public:
    using std::enable_shared_from_this<PointBalance>::shared_from_this;
    std::shared_ptr<UUID> userId;

    double allTimePoints;

    double weeklyPoints;

    double streakDays;

    any lastLoginDate;

    double level;

    string levelName;

    std::shared_ptr<Date> updatedAt;
};

class UserSummary : public object, public std::enable_shared_from_this<UserSummary> {
public:
    using std::enable_shared_from_this<UserSummary>::shared_from_this;
    std::shared_ptr<UUID> userId;

    double allTimePoints;

    double weeklyPoints;

    double streakDays;

    double level;

    string levelName;

    object nextMilestone;

    any lastLoginDate;

    double swapsCompleted;
};

class LeaderboardEntry : public object, public std::enable_shared_from_this<LeaderboardEntry> {
public:
    using std::enable_shared_from_this<LeaderboardEntry>::shared_from_this;
    double rank;

    std::shared_ptr<UUID> userId;

    double points;

    double level;

    string levelName;

    string username;

    string avatar;
};

class ReferralCode : public object, public std::enable_shared_from_this<ReferralCode> {
public:
    using std::enable_shared_from_this<ReferralCode>::shared_from_this;
    std::shared_ptr<UUID> userId;

    string code;

    any referrerId;

    any status;

    std::shared_ptr<Date> createdAt;
};

class ReferralStats : public object, public std::enable_shared_from_this<ReferralStats> {
public:
    using std::enable_shared_from_this<ReferralStats>::shared_from_this;
    double totalReferrals;

    double activatedReferrals;

    double totalPointsEarned;
};

class Campaign : public object, public std::enable_shared_from_this<Campaign> {
public:
    using std::enable_shared_from_this<Campaign>::shared_from_this;
    string id;

    string name;

    any actionType;

    double multiplier;

    std::shared_ptr<Date> startAt;

    std::shared_ptr<Date> endAt;

    boolean active;
};

#endif
