#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".target/types/otc.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * Security Audit Test Suite for OTC Program
 * 
 * These tests verify that security vulnerabilities have been properly fixed
 * and cover all critical paths through the program.
 */

  // Test keypairs

    // Create mints

    // Setup victim desk with inventory

    // Deposit tokens to victim desk

    // Register token for victim desk

    // Set prices

    // Deposit inventory

    // Add victim as approver

    // Setup attacker desk

    // Attacker adds themselves as approver

    // Register token on attacker desk with different price

    // Set MUCH LOWER price on attacker registry

      // Create legitimate offer on victim desk

      // Create offer on victim desk

      // ATTACK: Try to approve using attacker's desk (where attacker is approver)

      // Create and approve offer on victim desk

      // Get attacker USDC

      // ATTACK: Try to fulfill using wrong desk

      // Approve using correct desk

      // ATTACK: Try to create offer on victim desk but using attacker's cheap price registry

      // Create and complete an offer flow to test claim

      // Reset limits for longer expiry

      // Fund buyer with USDC

      // Fulfill offer

      // Create buyer token ATA

      // Create attacker token ATA to try stealing tokens

      // ATTACK: Try to claim tokens to attacker's ATA

      // Claim correctly - should work

        // Attacker tries to set price on victim's registry

      // Pause

      // Unpause

      // Unpause for other tests

      // 1. Create offer with discount

      // Verify offer state

      // 2. Approve offer

      // 3. Fund buyer and fulfill

      // 4. Claim tokens

      // Verify final state

      // Verify buyer received tokens

      // 1. Create offer for SOL payment

      // 2. Approve

      // 3. Fulfill with SOL

      // 4. Claim


} // namespace elizaos
