#include "..entityId.hpp"
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

// Quote Service - quote management for Eliza OTC Desk
// Single source of truth registered with runtime.getService("QuoteService")

class QuoteService extends Service {
  static serviceType = "QuoteService" as any;
  static serviceName = "QuoteService";

  get serviceType(): string {
    return "QuoteService";
  }

    // Cleanup if needed

    // Also index by beneficiary for faster lookups

    // Generate deterministic quote ID from entityId
    // Use a hash of entityId + current day to allow one quote per wallet per day

    // Token metadata (optional but recommended)

    // Check if quote already exists

      // Token metadata

    // Fetch all quotes in parallel for better performance

    // First try the beneficiary index (fast path)

      // Fetch indexed quotes in parallel

    // Fallback: search all quotes in parallel (slower but complete)

    // Fetch entity quotes in parallel

    // Search for unindexed quotes (in parallel)

      // Find quotes with matching entityId that weren't indexed

      // Fix the index in batch if we found any

    // Fetch all quotes in parallel

    // Remove from old entity's quote list

    // Remove from old beneficiary's index

    // Add to new entity's quote list

    // Add to new beneficiary's index

  // Helper: Get latest active quote by wallet address
    // Return the most recent ACTIVE quote only

  // Helper: Expire all active quotes for a user (called before creating new one)

    // Expire all quotes in parallel for better performance

// Helper to get service from runtime


} // namespace elizaos
