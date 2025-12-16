#include ".services/csv.hpp"
#include ".services/database.hpp"
#include ".services/solana.hpp"
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



struct PrefetchConfig {
    double batchSize;
    double delayBetweenBatches;
    double maxRetries;
    bool resumeFromFailures;
    bool forceRefresh;
    std::optional<double> limit;
};

struct PrefetchStats {
    double totalWallets;
    double processed;
    double successful;
    double failed;
    double skipped;
    Date startTime;
    std::optional<Date> endTime;
    std::vector<std::string> errors;
};

class PrefetchService {
  private config: PrefetchConfig;
  private stats: PrefetchStats;
  private isRunning: boolean = false;
  private shouldStop: boolean = false;

  constructor(config: Partial<PrefetchConfig> = {}) {
    this.config = {
      batchSize: 50, // Optimized for premium Helius endpoint (1000+ req/min)
      delayBetweenBatches: 200, // 200ms for premium endpoint
      maxRetries: 3,
      resumeFromFailures: true,
      forceRefresh: false,
      limit: undefined,
      ...config,
    };

    // If limit is set and it's for testing, adjust batch settings for faster processing
    if (this.config.limit && this.config.limit <= 20) {
      this.config.batchSize = Math.min(this.config.batchSize, 20);
      this.config.delayBetweenBatches = Math.min(
        this.config.delayBetweenBatches,
        100,
      );
    }

    this.stats = {
      totalWallets: 0,
      processed: 0,
      successful: 0,
      failed: 0,
      skipped: 0,
      startTime: new Date(),
      errors: [],
    };

    // Handle graceful shutdown
    process.on("SIGINT", this.handleShutdown.bind(this));
    process.on("SIGTERM", this.handleShutdown.bind(this));
  }

      // Wait for current operations to finish

    // Update system metrics

        // Check if wallet should be skipped

        // If resumeFromFailures is true, only process failed wallets

        // Fetch wallet data

        // Log error but continue processing

      // Get all unique tokens from successful wallets

      // Refresh prices in batches

        // Small delay between price batches

      // Load wallet data from CSV

      // Apply limit if specified

      // Process wallets in batches

        // Delay between batches (except for the last batch)

        // Refresh token prices

        // Cleanup database

      // Check CSV file

      // Check database connection

      // Check Solana service

      // Print current stats

// CLI interface
std::future<void> main();

// Run the script


} // namespace elizaos
