#include ".constants.hpp"
#include ".schema.hpp"
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



class GamificationService extends Service {
  static serviceType = 'gamification';
  capabilityDescription = 'Records points for user actions and provides gamification state';

  private getDb(): DatabaseAdapter | undefined {
    return (this.runtime as RuntimeWithDb).db;
  }

  /**
   * Check if a userId belongs to an agent (not a human user)
   */
    // Check if userId matches the agent's ID or character ID

    // Never award points to agents

      // Validate metadata size to prevent DB bloat (max 10KB)

      // Use transaction to ensure atomicity
        // Start transaction (if supported by adapter)

    // Agents should never have a summary (return empty summary)

    // Count swaps completed

    // Get all balances, then filter out agents

    // Filter out agents and limit results

    // Batch fetch entity data for display names and avatars (avoids N+1 queries)
    
    // Fetch all entities in parallel (single batch)
        // Entity not found or error fetching - use fallback

    // Build leaderboard entries using pre-fetched entity data

    // Agents should never have a rank

    // Count users with equal or higher points, excluding agents

    // Filter out agents and count

    // One-time only events (awarded once ever per user)

    // REFERRAL_SIGNUP should be once per referral relationship (check metadata.referredUserId)
      // Check if this referredUserId already exists in metadata

    // REFERRAL_ACTIVATION should be once per referral relationship (check metadata.activatedUserId)
      // Check if this activatedUserId already exists in metadata

    // DAILY_QUEST should be once per day

    // For MEANINGFUL_CHAT, use tier-based points from metadata

      // Award points directly without recursion to avoid infinite loops


} // namespace elizaos
