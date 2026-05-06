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



// ============================================================================
// Types
// ============================================================================

struct TokenBalance {
    std::string symbol;
    std::string name;
    std::string balance;
    std::string balanceFormatted;
    double usdValue;
    double usdPrice;
    string | null contractAddress;
    std::string chain;
    double decimals;
    std::optional<std::string> icon;
};

struct NFT {
    std::string chain;
    std::string contractAddress;
    std::string tokenId;
    std::string name;
    std::string description;
    std::string image;
    std::string contractName;
    std::string tokenType;
    std::optional<std::string> balance;
    std::vector<std::any> attributes;
};

struct Transaction {
    std::string chain;
    std::string hash;
    std::string from;
    std::string to;
    std::string value;
    std::string asset;
    std::string category;
    double timestamp;
    std::string blockNum;
    std::string explorerUrl;
    'sent' | 'received' direction;
    std::optional<string | null> icon;
    std::optional<string | null> contractAddress;
};

struct SwapPriceResult {
    bool liquidityAvailable;
    std::string toAmount;
    std::string minToAmount;
};

struct SwapResult {
    std::string transactionHash;
    std::string from;
    std::string fromToken;
    std::string toToken;
    std::string fromAmount;
    std::string toAmount;
    std::string network;
    std::string method;
};

struct SendResult {
    std::string transactionHash;
    std::string from;
    std::string to;
    std::string amount;
    std::string token;
    std::string network;
    std::string method;
};

struct SendNFTResult {
    std::string transactionHash;
    std::string from;
    std::string to;
    std::string contractAddress;
    std::string tokenId;
    std::string network;
};

// ============================================================================
// CDP Transaction Manager Class (Singleton)
// ============================================================================

class CdpTransactionManager {
  private static instance: CdpTransactionManager | null = null;
  
  private cdpClient: CdpClient | null = null;
  private tokensCache = new Map<string, CacheEntry<any>>();
  private nftsCache = new Map<string, CacheEntry<any>>();
  private iconCache = new Map<string, string | null>(); // Global icon cache: contractAddress -> iconUrl (null = no icon)
  private readonly CACHE_TTL = 300 * 1000; // 5 minutes

  // Private constructor to prevent direct instantiation
  private constructor() {
    this.initializeCdpClient();
  }

  /**
   * Get the singleton instance of CdpTransactionManager
   */

  // ============================================================================
  // Initialization
  // ============================================================================

  // ============================================================================
  // Icon Cache Helpers
  // ============================================================================

  /**
   * Get icon from global cache by contract address
   * Returns undefined if not in cache, null if cached as "no icon"
   */

  /**
   * Set icon in global cache by contract address
   * Accepts null to mark "no icon available" and prevent refetching
   */
    // Store even if icon is null/undefined to prevent refetching

  /**
   * Get icon for a contract address (check cache, then fetch if needed)
   */
    // Check cache first
      // Found in cache (could be null or a URL)

    // Not in cache - fetch token info to get icon
      // Cache null to prevent future attempts

      // Cache the result (even if null)
      // Cache null to prevent retries

  // ============================================================================
  // Wallet Operations
  // ============================================================================

  /**
   * Construct viem walletClient and publicClient for a given CDP account and network
   * Note: Uses toAccount() to convert CDP server-managed wallet for viem compatibility
   */

    // toAccount() allows viem to use CDP's server-managed wallet signing

  // ============================================================================
  // Token Operations
  // ============================================================================

    // Validate chain if provided

    // Check cache first (unless force sync)

    // Update cache

  /**
   * Get actual on-chain token balance for a specific token
   * This fetches the real-time balance directly from the blockchain
   * Use this for 100% swaps to ensure we use the exact on-chain balance
   * @param userId User's account identifier
   * @param network Network to check balance on
   * @param tokenAddress Token contract address (or native token address)
   * @param walletAddress Optional wallet address to avoid CDP account lookup
   */

    // Use provided wallet address or account address

    // Handle native token balance

    // Handle ERC20 token balance

  /**
   * Map our chain names to Alchemy Portfolio API network format
   */

    // If address not provided, get it from CDP account (for write operations or when entity metadata unavailable)
      
      // Validate address from account

    // Determine which networks to fetch
      // Validate the chain is supported
      // Check if it's a mainnet network

    // Convert our chain names to Alchemy network format

    // Use Alchemy Portfolio API to fetch tokens

    // Handle pagination

        // Process tokens from Alchemy response
            // Skip if there's an error in the token data

            // Skip tokens with 0 balance

            // Map Alchemy network back to our chain name

            // Extract token metadata

            // Determine if this is a native token
            // Native tokens typically have null tokenAddress or special zero address

            // If Alchemy didn't provide an icon and this is not a native token, try to fetch from CoinGecko
                  // Use cached value (could be null or a URL)
                  // Not in cache - try to fetch from CoinGecko
                  // Cache the result (even if null) to prevent repeated fetches
                // If fetch fails, cache null to prevent retries
              // Cache icon from Alchemy if available

            // Extract USD price according to Alchemy Portfolio API spec:
            // tokenPrices is an array of objects: [{ currency: string, value: string, lastUpdatedAt: string }]
            // Note: currency is lowercase "usd" not uppercase "USD"
            
              // Find USD price entry in the array (case-insensitive comparison)
                // value is a string according to API spec, parse it
              // Unexpected structure - log for debugging

            // Convert balance from hex string to number

            // Only add to total if it's a valid number

        // Cache per-chain results after processing this page (for progressive UI updates)
        // Group tokens by chain and cache each chain separately

    // Ensure totalUsdValue is a valid number

    // Cache aggregate result if fetching all chains

  // ============================================================================
  // NFT Operations
  // ============================================================================

    // If address not provided, get it from CDP account (for write operations or when entity metadata unavailable)
      
      // Validate address from account
  
    // Determine which networks to fetch
      // Validate the chain is supported
      // Check if it's a mainnet network
  
    // Fetch NFTs from specified networks using Alchemy REST API

          // Get image URL and handle IPFS

          // Populate icon cache with NFT image (for NFT contract addresses)

  // ============================================================================
  // Transaction History
  // ============================================================================

  /**
   * Helper: Extract timestamp from transaction data
   * Falls back to fetching block timestamp if metadata is missing
   */
    // Use blockTimestamp if available
    
    // Fallback: fetch block timestamp from blockNum
          // Block timestamp is in hex seconds, convert to milliseconds
    
    // Last resort: use current time

    // If address not provided, get it from CDP account (for write operations or when entity metadata unavailable)
      
      // Validate address from account

        // Fetch sent transactions
              // maxCount: '0x19',

        // Fetch received transactions
              // maxCount: '0x19',

              // Get icon from global cache or fetch if not found

              // Get icon from global cache or fetch if not found

  // ============================================================================
  // Send Operations
  // ============================================================================

      // Fallback to viem (CDP server-managed wallet via toAccount)

  // ============================================================================
  // Swap Operations
  // ============================================================================

    // Validate account address for price estimation
      // For price estimation, we can use a placeholder address or fall back to alternative methods
      // Use a zero address as placeholder for price estimation only

        // Fall through to 0x API / Uniswap V3 fallback

    // Pre-flight check: Verify token balance before attempting swap

              // Check ERC20 balance

        // Use networkAccount for CDP swap

        // Handle nonce issues - wait for pending transactions to clear and retry

                  // Wait up to 30 seconds for pending transactions to be mined

                  // Wait for CDP SDK nonce cache to sync
                  
                  // Retry CDP SDK swap after nonce cleared

        // Check if error is about token approval for Permit2

          // Get viem clients for approval transaction
          
          // ERC20 approve ABI
          
          // Approve max uint256 for Permit2

          // Wait for approval confirmation
          
          // Wait for CDP SDK's nonce cache to sync with on-chain state
          
          // Retry swap after approval using networkAccount

            // If retry still fails after Permit2 approval, fallback to 0x API

          // Fallback to 0x API for CDP-supported networks if not an approval issue

      // Non-CDP-supported networks: use 0x API

  // ============================================================================
  // Private Helper Methods - Transaction Confirmation
  // ============================================================================

  /**
   * Wait for transaction receipt and verify it didn't revert
   * @param transactionHash Transaction hash to wait for
   * @param network Network name
   * @param operationName Name of the operation (for logging)
   * @returns Transaction receipt
   * @throws Error if transaction reverted
   */

  // ============================================================================
  // Private Helper Methods - Swap Fallback
  // ============================================================================

  /**
   * Execute swap using 0x API
   */

  // ============================================================================
  // Private Helper Methods - Token Approval & Wrapping
  // ============================================================================

  // ============================================================================
  // Private Helper Methods - Uniswap
  // ============================================================================

    // Try MEDIUM fee tier first
  
    // Try LOW fee tier
  
    // Try HIGH fee tier as last resort
  
    // All fee tiers failed - no liquidity pool exists

    // Normalize token addresses

    // If swapping FROM native token, wrap it first
  
    // Approve token if needed
  
    // Get quote for slippage calculation
  
    // Calculate minimum amount out based on slippage tolerance
  
    // Prepare and execute swap

    // If swapping TO native token, unwrap it

  // ============================================================================
  // Private Helper Methods - 0x API
  // ============================================================================

      // Build base parameters

      // Add fee recipient (for monetization/affiliate fees)
      // Defaults to Otaku treasury address if not configured
      
      // swapFeeToken must be either buyToken or sellToken per 0x API docs

  /**
   * Execute swap using 0x API v2
   */

    // Build base parameters

    // Add fee recipient (for monetization/affiliate fees)
    // Defaults to Otaku treasury address if not configured
    
    // swapFeeToken must be either buyToken or sellToken per 0x API docs

    // Check if quote has expired (0x quotes typically expire after a few seconds)

      // Warn if quote is about to expire (less than 10 seconds)

    // Double-check quote hasn't expired right before execution

  // ============================================================================
  // Token Search Operations
  // ============================================================================

    // Map chain names to CoinGecko platform IDs

    // Check if query is a contract address

      // Search by contract address

            // Get decimals from CoinGecko's detail_platforms

      // Fallback to DexScreener if CoinGecko didn't find the token

      // Search by symbol or name using search endpoint

        // Get detailed info for top results (limit to 10 for performance)

              // Find contract address for the requested chain or any supported chain

                // Get first available supported chain

                // Get decimals from CoinGecko's detail_platforms

  // ============================================================================
  // Private Helper Methods - Token Info & Utilities
  // ============================================================================

        return parseFloat(`0.${zeros}${balanceStr}`);
        return parseFloat(`${intPart}.${fracPart}`);

  /**
   * Get top tokens by market cap and trending tokens for a specific chain
   */

    // Map chain names to CoinGecko category IDs

    // Fetch top tokens by market cap - get contract addresses for swap functionality

          // Map chain names to CoinGecko platform IDs

          // Fetch contract addresses for top tokens (limit to avoid too many calls)
          
          // Batch fetch coin details to get contract addresses
          // Use Promise.allSettled to handle failures gracefully

    // Fetch trending tokens from CoinGecko

        // Map chain names to CoinGecko platform IDs

        // Get top trending coins (limit to avoid too many API calls)
        
        // Fetch contract addresses for trending tokens

                // Get current price from markets endpoint


} // namespace elizaos
