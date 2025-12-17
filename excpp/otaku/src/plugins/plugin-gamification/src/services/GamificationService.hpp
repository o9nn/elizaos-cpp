#pragma once
#include <any>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include ".constants.hpp"
#include ".schema.hpp"
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



class GamificationService {
public:
    DatabaseAdapter getDb();
    bool isAgent(UUID userId);
    static std::future<GamificationService> start(IAgentRuntime runtime);
    std::variant<Promise<PointBalance, null>> recordEvent(GamificationEventInput event);
    std::future<UserSummary> getUserSummary(UUID userId);
    std::future<std::vector<LeaderboardEntry>> getLeaderboard(const std::string& scope, auto limit);
    std::future<double> getUserRank(UUID userId, const std::string& scope);
    std::future<bool> enforceRateLimits(UUID userId, GamificationEventType actionType, std::optional<std::unordered_map<std::string, std::any>> metadata);
    std::future<double> calculatePoints(GamificationEventInput event);
    std::future<double> applyActiveCampaigns(GamificationEventType actionType, double basePoints);
    std::future<PointBalance> updateBalance(UUID userId, double points, GamificationEventType actionType);
    std::future<PointBalance> getBalance(UUID userId);
    std::future<void> checkFirstChainBonus(UUID userId, const std::string& chain);
     getLevelInfo(double points);
     getNextMilestone(double points);
    std::future<void> emitPointsAwarded(UUID userId, const std::any& payload);
    std::future<void> stop();
};


} // namespace elizaos
