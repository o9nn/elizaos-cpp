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
 * Drizzle ORM schema for gamification tables
 */

/**
 * Append-only ledger of all gamification events
 */
  // Composite index for common query pattern: userId + actionType + createdAt

/**
 * Materialized point balances per user
 */

/**
 * User referral codes
 */

/**
 * First-time chain transaction tracking
 */

/**
 * Campaign multipliers configuration
 */

/**
 * Denormalized leaderboard snapshots for fast reads
 */

/**
 * Export schema object for plugin registration
 */


} // namespace elizaos
