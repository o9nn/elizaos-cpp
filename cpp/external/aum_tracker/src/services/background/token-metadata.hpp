#include ".database.hpp"
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



struct TokenMetadataRequest {
    std::string mint;
    double priority;
    double retryCount;
    Date requestedAt;
};

struct TokenMetadataResponse {
    std::string symbol;
    std::string name;
    std::optional<std::string> logoURI;
};

class TokenMetadataService {
  private db: DatabaseService;
  private jupiterTokensApiUrl: string;
  private requestQueue: TokenMetadataRequest[] = [];
  private isProcessing = false;
  private maxRetries = 3;
  private baseDelay = 1000;
  private requestDelay = 500; // 500ms between requests to respect rate limits
  private maxConcurrentRequests = 1; // Process one at a time
  private shutdownRequested = false;
  private batchStats = {
    processed: 0,
    successful: 0,
    failed: 0,
    batchStartTime: 0,
  };

    // Setup graceful shutdown

  // Add token to metadata fetch queue
    // Check if already in queue
      // Update priority if higher

    // Check if we already have recent metadata

    // Sort queue by priority (high to low)

    // Silently add to queue to reduce log verbosity

    // Start processing if not already running

  // Add multiple tokens to queue

  // Check if metadata is fresh (less than 24 hours old)

  // Start processing the queue

    // Reset batch stats

        // Retry logic
          // Add back to queue with lower priority

      // Delay between requests to respect rate limits

    // Print batch summary if any tokens were processed

  // Process individual token metadata

      // Store metadata in database

      // Log successful fetch

      // Successfully fetched metadata silently
      // Log error

  // Retry mechanism with exponential backoff

  // Get queue status

  // Clear the queue

  // Get tokens that need metadata refresh

  // Refresh stale metadata

  // Health check

      // Test Jupiter Token API

  // Graceful shutdown

    // Wait for current processing to complete

  // Start background metadata refresh (run periodically)

  // Print batch processing summary

  // Get current batch processing stats

// Create singleton instance

} // namespace elizaos
