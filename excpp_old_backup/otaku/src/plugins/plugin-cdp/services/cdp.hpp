#include ".types.hpp"
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



struct WalletToken {
    std::string symbol;
    std::string name;
    std::string balance;
    std::string balanceFormatted;
    double usdValue;
    double usdPrice;
    string | null contractAddress;
    std::string chain;
    double decimals;
};

struct WalletNFT {
    std::string chain;
    std::string contractAddress;
    std::string tokenId;
    std::string name;
    std::string description;
    std::string contractName;
    std::string tokenType;
    std::optional<std::string> balance;
    std::optional<std::vector<unknown>> attributes;
};

struct WalletInfo {
    std::string address;
    std::vector<WalletToken> tokens;
    std::vector<WalletNFT> nfts;
    double totalUsdValue;
};

class CdpService extends Service {
  static serviceType = "CDP_SERVICE";
  capabilityDescription = "Provides authenticated access to Coinbase CDP via Transaction Manager";

  private transactionManager: CdpTransactionManager;

  constructor(runtime: IAgentRuntime) {
    super(runtime);
    this.transactionManager = CdpTransactionManager.getInstance();
  }

  /**
   * Get or create wallet for account
   * Delegates to transaction manager
   */

  /**
   * Get Viem wallet and public clients for a CDP account on a specific network
   * Delegates to transaction manager
   */

  /**
   * Get comprehensive wallet information from cache if available and not expired
   * Falls back to fetching fresh data if cache miss or expired
   * Delegates to transaction manager (uses manager's 5-minute cache)
   * @param accountName User's account identifier
   * @param chain Optional specific chain to fetch (if not provided, fetches all chains)
   */

    // Use manager's cache (5-minute TTL)
    // Pass address if available to avoid CDP account lookup

  /**
   * Fetch fresh wallet information, bypassing cache
   * Use this when you need the most up-to-date wallet state
   * Delegates to transaction manager with forceSync=true
   * @param accountName User's account identifier
   * @param chain Optional specific chain to fetch (if not provided, fetches all chains)
   * @param address Optional wallet address to avoid CDP account lookup
   */

    // Force sync - bypass manager's cache
    // Pass address if available to avoid CDP account lookup

  /**
   * Transfer tokens from CDP wallet
   * Delegates to transaction manager
   */

  /**
   * Execute token swap with automatic fallback to multiple swap providers
   * Delegates to transaction manager
   * 
   * Fallback chain (handled by manager):
   * 1. CDP SDK (for supported networks) with Permit2 approval handling
   * 2. 0x API v2 (if configured)
   * 3. Uniswap V3 (direct protocol interaction)
   * 
   * Reference: https://docs.cdp.coinbase.com/trade-api/quickstart#3-execute-a-swap
   */

  /**
   * Get swap price estimate
   * Delegates to transaction manager
   */

  /**
   * Transfer NFT from CDP wallet
   * Delegates to transaction manager
   */

  /**
   * Get actual on-chain token balance for a specific token
   * This fetches the real-time balance directly from the blockchain
   * Use this for 100% swaps to ensure we use the exact on-chain balance
   * @param accountName User's account identifier
   * @param network Network to check balance on
   * @param tokenAddress Token contract address (or native token address)
   * @param walletAddress Optional wallet address to avoid CDP account lookup
   */


} // namespace elizaos
