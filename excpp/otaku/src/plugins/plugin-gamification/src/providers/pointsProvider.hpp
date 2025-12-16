#include ".services/GamificationService.hpp"
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

import type { Provider, IAgentRuntime, Memory, State, ProviderResult } from '@elizaos/core';
;

const pointsProvider: Provider = {
  name: 'USER_POINTS',
  description: 'Provides current user points, level, and streak for agent awareness',

  dynamic: true,
  get: async (runtime: IAgentRuntime, message: Memory, state?: State): Promise<ProviderResult> => {
    try {
      const gamificationService = runtime.getService('gamification') as GamificationService;
      if (!gamificationService) {
        return {
          text: 'Gamification service not available',
          values: {},
        };
      }

      const summary = await gamificationService.getUserSummary(message.entityId);

      return {
        text: `User has ${summary.allTimePoints.toLocaleString()} total points (Level: ${summary.levelName}, Streak: ${summary.streakDays} days)`,
        values: {
          points: summary.allTimePoints,
          weeklyPoints: summary.weeklyPoints,
          level: summary.level,
          levelName: summary.levelName,
          streak: summary.streakDays,
        },
        data: {
          points: summary.allTimePoints,
          weeklyPoints: summary.weeklyPoints,
          level: summary.level,
          levelName: summary.levelName,
          streak: summary.streakDays,
        },
      };
    } catch (error) {
      return {
        text: 'Unable to fetch user points',
        values: {},
      };
    }
  },
};


} // namespace elizaos
