#pragma once
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "constants.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * TypeScript interfaces for gamification system
 */

/**
 * Input for recording a gamification event
 */
struct GamificationEventInput {
    UUID userId;
    GamificationEventType actionType;
    std::optional<UUID> sourceEventId;
    std::optional<double> volumeUsd;
    std::optional<std::string> chain;
};

/**
 * Point balance record
 */
struct PointBalance {
    UUID userId;
    double allTimePoints;
    double weeklyPoints;
    double streakDays;
    std::optional<std::chrono::system_clock::time_point> lastLoginDate;
    double level;
    std::string levelName;
    std::chrono::system_clock::time_point updatedAt;
};

/**
 * User summary with gamification stats
 */
struct UserSummary {
    UUID userId;
    double allTimePoints;
    double weeklyPoints;
    double streakDays;
    double level;
    std::string levelName;
    double level;
    std::string levelName;
    double pointsNeeded;
    std::optional<std::chrono::system_clock::time_point> lastLoginDate;
    std::optional<double> swapsCompleted;
};

/**
 * Leaderboard entry
 */
struct LeaderboardEntry {
    double rank;
    UUID userId;
    double points;
    double level;
    std::string levelName;
    std::optional<std::string> username;
    std::optional<std::string> avatar;
};

/**
 * Referral code record
 */
struct ReferralCode {
    UUID userId;
    std::string code;
    std::optional<UUID> referrerId;
    std::string status;
    std::chrono::system_clock::time_point createdAt;
};

/**
 * Referral stats
 */
struct ReferralStats {
    double totalReferrals;
    double activatedReferrals;
    double totalPointsEarned;
};

/**
 * Campaign configuration
 */
struct Campaign {
    std::string id;
    std::string name;
    std::optional<GamificationEventType> actionType;
    double multiplier;
    std::chrono::system_clock::time_point startAt;
    std::chrono::system_clock::time_point endAt;
    bool active;
};


} // namespace elizaos
