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

/**
 * Gamification constants - point values, caps, and configuration
 */

enum GamificationEventType {
  ACCOUNT_CREATION = 'ACCOUNT_CREATION',
  DAILY_LOGIN_STREAK = 'DAILY_LOGIN_STREAK',
  MEANINGFUL_CHAT = 'MEANINGFUL_CHAT',
  SWAP_COMPLETED = 'SWAP_COMPLETED',
  BRIDGE_COMPLETED = 'BRIDGE_COMPLETED',
  TRANSFER_COMPLETED = 'TRANSFER_COMPLETED',
  DAILY_QUEST = 'DAILY_QUEST',
  REFERRAL_SIGNUP = 'REFERRAL_SIGNUP',
  REFERRAL_ACTIVATION = 'REFERRAL_ACTIVATION',
  REFERRED_WELCOME = 'REFERRED_WELCOME',
  FIRST_CHAIN_BONUS = 'FIRST_CHAIN_BONUS',
  AGENT_ACTION = 'AGENT_ACTION',
}

/**
 * Base point values for each action type
 */

/**
 * Daily caps per action type
 */

/**
 * Volume-based bonus multipliers
 */

/**
 * Level thresholds
 */

/**
 * Streak bonus calculation
 * Base: 25 points
 * Bonus: +10 per consecutive day (max +70)
 * Total max: 95 points per day
 */

/**
 * Minimum transfer value for points (USD)
 */

/**
 * Minimum chat message length for "meaningful" chat (characters)
 */

/**
 * Message length tiers for chat points
 * Messages < 25 chars earn 0 points
 */

/**
 * Daily quest requirements
 */

/**
 * Referral activation window (days)
 */


} // namespace elizaos
