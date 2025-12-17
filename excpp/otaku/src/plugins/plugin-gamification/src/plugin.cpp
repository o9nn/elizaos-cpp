#include "plugin.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> handleGetLeaderboard(Request req, Response res, IAgentRuntime runtime) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto scope = (req.query.scope as 'weekly' | 'all_time') || 'weekly';

        // Validate and limit input
        const auto rawLimit = parseInt(req.query.limit) || 50;
        const auto limit = Math.min(Math.max(1, rawLimit), 100); // Clamp between 1 and 100;

        const auto userId = req.query.userId | std::nullopt;

        // Validate scope
        if (scope != 'weekly' && scope != 'all_time') {
            return res.status(400).json({ error: 'Invalid scope. Must be "weekly" or "all_time"' });
        }

        const auto gamificationService = runtime.getService('gamification');
        if (!gamificationService) {
            return res.status(503).json({ error: 'Gamification service not available' });
        }

        const auto entries = gamificationService.getLeaderboard(scope, limit);

        auto userRank = 0;
        if (userId) {
            userRank = gamificationService.getUserRank(userId, scope);
        }

        res.json({
            scope,
            entries,
            userRank,
            limit,
            });
            } catch (error) {
                std::cerr << { error } << '[GamificationPlugin] Error fetching leaderboard' << std::endl;
                res.status(500).json({ error: 'Error fetching leaderboard' });
            }

}

std::future<void> handleGetUserSummary(Request req, Response res, IAgentRuntime runtime) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto userId = req.query.userId | std::nullopt;

        if (!userId) {
            return res.status(400).json({ error: 'userId is required' });
        }

        const auto gamificationService = runtime.getService('gamification');
        if (!gamificationService) {
            return res.status(503).json({ error: 'Gamification service not available' });
        }

        const auto summary = gamificationService.getUserSummary(userId);
        res.json(summary);
        } catch (error) {
            std::cerr << { error } << '[GamificationPlugin] Error fetching user summary' << std::endl;
            res.status(500).json({ error: 'Error fetching user summary' });
        }

}

std::future<void> handleGetReferralCode(Request req, Response res, IAgentRuntime runtime) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto userId = req.query.userId | std::nullopt;

        if (!userId) {
            return res.status(400).json({ error: 'userId is required' });
        }

        const auto referralService = runtime.getService('referral');
        if (!referralService) {
            return res.status(503).json({ error: 'Referral service not available' });
        }

        const auto { code, stats } = referralService.getOrCreateCode(userId);
        "https://otaku.so/?ref=" + std::to_string(code)
        } catch (error) {
            std::cerr << { error } << '[GamificationPlugin] Error fetching referral code' << std::endl;
            res.status(500).json({ error: 'Error fetching referral code' });
        }

}

} // namespace elizaos
