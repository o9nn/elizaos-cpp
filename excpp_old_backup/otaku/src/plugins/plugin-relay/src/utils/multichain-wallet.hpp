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



// Map of chain IDs to viem Chain objects

// Default RPC URLs for each chain

/**
 * Multi-chain wallet adapter for Relay SDK
 * Dynamically creates wallet clients for each chain as needed
 */
class MultiChainWallet {
  private account: Account;
  private currentChainId: number;
  private walletClients: Map<number, WalletClient> = new Map();
  private defaultRpcUrl?: string;

  constructor(account: Account, defaultRpcUrl?: string, initialChainId: number = base.id) {
    this.account = account;
    this.currentChainId = initialChainId;
    this.defaultRpcUrl = defaultRpcUrl;
  }

  /**
   * Get or create a wallet client for the specified chain
   */

      // Use chain-specific RPC URL, not the default one

  /**
   * Get the current chain ID
   */

  /**
   * Switch to a different chain
   */

  /**
   * Get the wallet address
   */

  /**
   * Get the current wallet client for the active chain
   */

  /**
   * Get the underlying account
   */

/**
 * Create a Relay-compatible wallet adapter from an account
 */
MultiChainWallet createMultiChainWallet(Account account, std::optional<std::string> defaultRpcUrl);


} // namespace elizaos
