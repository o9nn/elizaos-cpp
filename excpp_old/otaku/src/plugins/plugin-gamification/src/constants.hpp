#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * Gamification constants - point values, caps, and configuration
 */

enum GamificationEventType {
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
