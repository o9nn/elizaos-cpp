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

import type { Plugin } from '@elizaos/core';
;
import type { IAgentRuntime } from '@elizaos/core';
import type { Request, Response } from 'express';
;
;
;
;
;
;
;
;
;
;

// Leaderboard route handlers
async );
    }

    const gamificationService = runtime.getService('gamification') as GamificationService;
    if (!gamificationService) {
      return res.status(503).json({ error: 'Gamification service not available' });
    }

    const entries = await gamificationService.getLeaderboard(scope, limit);
    
    let userRank = 0;
    if (userId) {
      userRank = await gamificationService.getUserRank(userId as any, scope);
    }

    res.json({
      scope,
      entries,
      userRank,
      limit,
    });
  } catch (error) {
    logger.error({ error }, '[GamificationPlugin] Error fetching leaderboard');
    res.status(500).json({ error: 'Error fetching leaderboard' });
  }
}

// User summary route handler
async );
    }

    const gamificationService = runtime.getService('gamification') as GamificationService;
    if (!gamificationService) {
      return res.status(503).json({ error: 'Gamification service not available' });
    }

    const summary = await gamificationService.getUserSummary(userId as any);
    res.json(summary);
  } catch (error) {
    logger.error({ error }, '[GamificationPlugin] Error fetching user summary');
    res.status(500).json({ error: 'Error fetching user summary' });
  }
}

// Referral code route handler
async );
    }

    const referralService = runtime.getService('referral') as ReferralService;
    if (!referralService) {
      return res.status(503).json({ error: 'Referral service not available' });
    }

    const { code, stats } = await referralService.getOrCreateCode(userId as any);
    res.json({ code, stats, referralLink: `https://otaku.so/?ref=${code}` });
  } catch (error) {
    logger.error({ error }, '[GamificationPlugin] Error fetching referral code');
    res.status(500).json({ error: 'Error fetching referral code' });
  }
}

const gamificationPlugin: Plugin = {
  name: 'gamification',
  description: 'Points economy, leaderboards, and referral system for Otaku',

  schema: gamificationSchema,

  async init() {
    logger.info('*** Initializing Gamification plugin ***');
  },

  services: [GamificationService, ReferralService, LeaderboardService],

  actions: [getPointsSummaryAction, getReferralCodeAction, getLeaderboardAction],

  providers: [pointsProvider, leaderboardProvider],

  events: gamificationEvents,

  // Note: Plugin routes are automatically protected by the general API rate limit middleware
  // (1000 requests per 15 minutes per IP) applied in createApiRouter.
  // Additional input validation is performed in the handlers themselves.
  routes: [
    {
      path: '/leaderboard',
      type: 'GET',
      handler: handleGetLeaderboard,
    },
    {
      path: '/summary',
      type: 'GET',
      handler: handleGetUserSummary,
    },
    {
      path: '/referral',
      type: 'GET',
      handler: handleGetReferralCode,
    },
  ],
};

default gamificationPlugin;


} // namespace elizaos
