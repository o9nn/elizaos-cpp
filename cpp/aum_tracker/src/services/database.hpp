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



struct WalletBalance {
    std::string wallet_address;
    std::string wallet_id;
    double sol_balance;
    string; // JSON string tokens;
    std::string last_updated;
    "success" | "error" | "pending" fetch_status;
    std::optional<std::string> error_message;
    double retry_count;
};

struct TokenPrice {
    std::string mint;
    std::string symbol;
    std::string name;
    double price;
    std::optional<double> price_change_24h;
    std::optional<double> market_cap;
    std::optional<std::string> image_url;
    std::string last_updated;
    "jupiter" | "coingecko" | "fallback" | "jupiter-metadata" source;
};

struct FetchLog {
    std::optional<double> id;
    std::string wallet_address;
    std::string timestamp;
    "balance" | "tokens" | "prices" | "token-metadata" operation;
    "success" | "error" | "timeout" status;
    std::optional<std::string> error_details;
    double response_time_ms;
    std::optional<std::string> details;
};

struct SystemMetric {
    std::string metric_name;
    std::string metric_value;
    std::string last_updated;
};

struct PortfolioSnapshot {
    std::optional<double> id;
    std::string wallet_address;
    std::string snapshot_timestamp;
    double sol_balance;
    string; // JSON string tokens;
    double total_usd_value;
    "hourly" | "daily" | "manual" snapshot_type;
};

struct WalletPNL {
    std::string wallet_address;
    double initial_value_usd;
    double current_value_usd;
    double realized_pnl_usd;
    double unrealized_pnl_usd;
    double total_pnl_usd;
    double total_pnl_percentage;
    std::string first_snapshot_date;
    std::string last_updated;
};

struct TokenPNL {
    std::optional<double> id;
    std::string wallet_address;
    std::string token_mint;
    double initial_amount;
    double current_amount;
    double initial_price_usd;
    double current_price_usd;
    double realized_pnl_usd;
    double unrealized_pnl_usd;
    double total_pnl_usd;
    std::string last_updated;
};

class DatabaseService {
  private db: Database;
  private static instance: DatabaseService;

  private constructor() {
    const dbPath = path.join(process.cwd(), "data", "portfolio.db");
    this.db = new Database(dbPath);
    this.initializeDatabase();
  }

    // Enable WAL mode for better performance

    // Create wallet_balances table

    // Create token_prices table

    // Add image_url column if it doesn't exist (migration)
      // Column already exists, ignore error

    // Add details column to fetch_logs if it doesn't exist (migration)
      // Column already exists, ignore error

    // Create fetch_logs table

    // Create system_metrics table

    // Create portfolio_snapshots table

    // Create wallet_pnl table

    // Create token_pnl table

    // Create indexes for better performance

    // Create indexes for PNL tables

    // Initialize system metrics

  // Wallet Balance Operations

  // Token Price Operations

  // Fetch Log Operations

  // System Metrics Operations

  // Portfolio Analytics

    // Calculate unique tokens and USD value

    // Get SOL price to calculate total USD value

    // Calculate total USD value including SOL

  // Utility Methods
    // Clean up old fetch logs (keep last 1000 entries)

    // Clean up old portfolio snapshots (keep last 365 days)

    // Keep daily snapshots for longer (2 years)

    // Optimize database

  // Portfolio Snapshot Operations

  // Wallet PNL Operations

  // Token PNL Operations

  // Combined PNL Analytics

    // Get top gainers and losers by token

  // PNL Calculation Methods

  // Health check

// Export singleton instance

} // namespace elizaos
