#include ".constants.hpp"
#include ".schema.hpp"
#include "GamificationService.hpp"
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



class ReferralService extends Service {
  static serviceType = 'referral';
  capabilityDescription = 'Manages referral codes and attribution';

  private getDb(): DatabaseAdapter | undefined {
    return (this.runtime as RuntimeWithDb).db;
  }

  /**
   * Get or create referral code for user
   */

    // Generate new code

  /**
   * Get referral stats for a user
   */

    // Count total referrals

    // Count activated referrals (users who completed first on-chain action)
    // Filter by userId to only count this user's referral activations

    // Calculate total points earned from referrals
    // Filter by userId to only count this user's referral signup points

  /**
   * Process referral signup
   */

    // Find referrer

    // Check if user already has a referral code (prevent self-referral)

    // Create referral code for new user

    // Award points to referrer

    // Award welcome bonus to new user

  /**
   * Process referral activation (first on-chain action)
   */

    // Find referrer

    // Check if this specific user has already been activated for this referrer

    // Check if this specific userId is already in any activation event metadata

    // Award activation bonus to referrer

  /**
   * Generate unique referral code from user ID using cryptographically secure randomness
   */
    // Use first 8 characters of UUID + cryptographically secure random suffix
    
    // Use crypto.randomBytes for secure randomness instead of Math.random()


} // namespace elizaos
