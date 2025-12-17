#pragma once
#include <any>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "cache.hpp"
#include "db.hpp"
#include "mcap.hpp"
#include "solana.hpp"
#include "tokenSupplyHelpers.hpp"
#include "tokenSupplyHelpers/customWallet.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Converts a decimal fee (e.g., 0.05 for 5%) to basis points (5% = 500 basis points)
 */
double convertToBasisPoints(double feePercent);

/**
 * Calculates the amount of SOL received when selling tokens
 */
double calculateAmountOutSell(double reserveLamport, double amount, double _tokenDecimals, double platformSellFee, double reserveToken);

double calculateAmountOutBuy(double reserveToken, double amount, double _solDecimals, double reserveLamport, double platformBuyFee);

// Type definition for token metadata from JSON
struct TokenMetadataJson {
    std::string name;
    std::string symbol;
    std::string description;
    std::string image;
    std::optional<std::string> twitter;
    std::optional<std::string> telegram;
    std::optional<std::string> farcaster;
    std::optional<std::string> website;
    std::optional<std::string> discord;
};

/**
 * Fetches metadata with exponential backoff retry
 */

std::future<void> getTxIdAndCreatorFromTokenAddress(const std::string& tokenAddress);

/**
 * Creates a new token record with all required data
 */

/**
 * Updates a list of token objects with calculated market data
 * @param tokens Array of token objects from database
 * @param env Cloudflare worker environment
 * @returns Array of tokens with updated market data
 */

  // Create compute budget instructions

  // Get the transaction

  // Add compute budget instructions at the beginning

  // Apply platform fee

  // Calculate expected output

    // Buy
    // Sell

  // Apply slippage to estimated output

// Get RPC URL based on the environment
  // Extract the base URL and ensure we use the correct API key

    // Default to mainnet

  // Use API key from environment, ensuring it's applied correctly

// Get mainnet RPC URL regardless of environment setting
  // Extract base URL and API key

// Get devnet RPC URL regardless of environment setting
  // Extract base URL and API key

// Generate a logger that works with Cloudflare Workers

// Execute a transaction
    //  Sign the transaction with payer wallet

    // Serialize, send and confirm the transaction

      // Simulate before sending

      // Wait for confirmation

      // Check if we got ProgramFailedToComplete but program actually succeeded
        // Get transaction logs to verify actual execution

std::vector<std::string> splitIntoLines(std::optional<std::string> text);

/**
 * Gets the maximum values needed for featured sorting
 *
 * @param db Database instance
 * @returns Object containing maxVolume and maxHolders values for normalization
 */
std::future<void> getFeaturedMaxValues(const std::any& db);

/**
 * Creates a SQL expression for calculating the weighted featured score
 *
 * @param maxVolume Maximum volume value for normalization
 * @param maxHolders Maximum holder count for normalization
 * @returns SQL expression for calculating the weighted score
 */
void getFeaturedScoreExpression(double maxVolume, double maxHolders);

/**
 * Calculates the weighted score for a token using JavaScript
 * This function matches the SQL logic for consistency
 *
 * @param token Token object with volume24h and holderCount properties
 * @param maxVolume Maximum volume value for normalization
 * @param maxHolders Maximum holder count for normalization
 * @returns Calculated weighted score
 */
double calculateFeaturedScore(std::optional<std::any> token, double maxVolume, double maxHolders);

/**
 * Applies a weighted sort for the "featured" tokens
 * Uses 70% weight on volume24h and 30% weight on holderCount
 *
 * @param tokensQuery Current tokens query that needs sorting applied
 * @param maxVolume Maximum volume value for normalization
 * @param maxHolders Maximum holder count for normalization
 * @param sortOrder Sort direction ("asc" or "desc")
 * @returns Updated tokens query with the weighted sorting applied
 */
void applyFeaturedSort(const std::any& tokensQuery, double maxVolume, double maxHolders, const std::string& sortOrder);


} // namespace elizaos
