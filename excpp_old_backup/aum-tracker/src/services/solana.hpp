#include "background/token-metadata.hpp"
#include "database.hpp"
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



struct TokenAccount {
    std::string mint;
    double amount;
    double decimals;
    std::optional<std::string> symbol;
    std::optional<std::string> name;
    std::optional<double> usdValue;
};

struct WalletData {
    std::string address;
    double solBalance;
    std::vector<TokenAccount> tokens;
    double totalUsdValue;
    std::string lastUpdated;
};

struct PriceData {
    double price;
    std::string symbol;
    std::string name;
    std::optional<double> change24h;
    std::optional<double> marketCap;
};

class SolanaService {
  private connection: Connection;
  private fallbackConnection: Connection;
  private rpcUrl: string;
  private jupiterApiUrl: string;
  private jupiterTokensApiUrl: string;
  private requestQueue: Array<() => Promise<any>> = [];
  private isProcessingQueue: boolean = false;
  private rateLimitDelay: number = 100; // 100ms between requests for premium Helius
  private maxRetries: number = 3;

  constructor() {
    // Helius RPC endpoint from environment variable
    this.rpcUrl = process.env.HELIUS_RPC_URL || "";

    if (!this.rpcUrl) {
      throw new Error("HELIUS_RPC_URL environment variable is required");
    }

    // Initialize connection using Helius RPC endpoint
    this.connection = new Connection(this.rpcUrl, {
      commitment: "confirmed",
      httpHeaders: {
        "Content-Type": "application/json",
      },
    });

    // Fallback to public RPC if Helius fails
    this.fallbackConnection = new Connection(clusterApiUrl("mainnet-beta"), {
      commitment: "confirmed",
    });

    this.jupiterApiUrl =
      process.env.JUPITER_API_URL || "https://lite-api.jup.ag/price/v2";
    this.jupiterTokensApiUrl =
      process.env.JUPITER_TOKENS_API_URL || "https://lite-api.jup.ag/tokens/v1";
  }

  // Rate limiting queue management

          await operation();

        // Rate limiting delay (Premium Helius can handle 1000+ req/min)

  // Retry wrapper with exponential backoff

  // Get SOL balance for a wallet using Helius

            // Use Helius connection for better rate limits

      // Log successful fetch

      // Log error

  // Get ALL SPL token accounts for a wallet using Helius enhanced RPC

            // Use Helius connection for better rate limits and enhanced data

      // Process ALL token accounts found with CORRECT decimals

      // Log successful fetch

      // Log error

  // Get token metadata from cache or queue for background fetching

    // Check cache first
        // Set default metadata for immediate use

    // Queue missing tokens for background fetching with high priority

  // Get SOL price from Jupiter API

        return parseFloat(data.data?.[SOL_MINT]?.price || "0");

      // Store SOL price in database

      // Return cached price if available

  // Get token prices from Jupiter API (keep existing implementation)

      // Always include SOL in price fetches

      // Fetch prices with retry, get metadata from cache/queue

        // Store in database with proper metadata

      // Log successful fetch

      // Log error

  // Get complete wallet data (SOL + ALL tokens + prices) with enhanced Helius integration
      // Get SOL balance and ALL token accounts in parallel

      // Get metadata and prices for ALL tokens found

      // Enhance token data with metadata and prices

      // Store in database

      // Store error in database

  // Batch process multiple wallets with improved error handling

      // Shorter delay between batches with Helius

  // Refresh stale prices

  // Get cached token prices

  // Health check with Helius RPC endpoint testing

      // Test Helius RPC connection

      // Test Jupiter API

  // Validate Solana address
      new PublicKey(address);

// Export singleton instance

} // namespace elizaos
