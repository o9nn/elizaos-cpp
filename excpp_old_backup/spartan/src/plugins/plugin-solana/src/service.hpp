#include "constants.hpp"
#include "elizaos/core.hpp"
#include "keypairUtils.hpp"
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



/**
 * Service class for interacting with the Solana blockchain and accessing wallet data.
 * @extends Service
 */
class SolanaService extends Service {
  static serviceType: string = SOLANA_SERVICE_NAME;
  capabilityDescription =
    'The agent is able to interact with the Solana blockchain, and has access to the wallet data';

  private updateInterval: NodeJS.Timer | null = null;
  private lastUpdate = 0;
  private readonly UPDATE_INTERVAL = 120000; // 2 minutes
  private connection: Connection;
  private publicKey: PublicKey;
  private exchangeRegistry: Record<number, any> = {};

  /**
   * Constructor for creating an instance of the class.
   * @param {IAgentRuntime} runtime - The runtime object that provides access to agent-specific functionality.
   */
    // Initialize publicKey using getWalletKey

  /**
   * Gets the wallet keypair for operations requiring private key access
   * @returns {Promise<Keypair>} The wallet keypair
   * @throws {Error} If private key is not available
   */

  /**
   * Starts the Solana service with the given agent runtime.
   *
   * @param {IAgentRuntime} runtime - The agent runtime to use for the Solana service.
   * @returns {Promise<SolanaService>} The initialized Solana service.
   */

    // Initial update

  /**
   * Stops the Solana service.
   *
   * @param {IAgentRuntime} runtime - The agent runtime.
   * @returns {Promise<void>} - A promise that resolves once the Solana service has stopped.
   */

  /**
   * Stops the update interval if it is currently running.
   * @returns {Promise<void>} A Promise that resolves when the update interval is stopped.
   */
    // Unsubscribe from all accounts

  /**
   * Fetches data from the provided URL with retry logic.
   * @param {string} url - The URL to fetch data from.
   * @param {RequestInit} [options={}] - The options for the fetch request.
   * @returns {Promise<unknown>} - A promise that resolves to the fetched data.
   */

  /**
   * Asynchronously fetches the prices of SOL, BTC, and ETH tokens.
   * Uses cache to store and retrieve prices if available.
   * @returns A Promise that resolves to an object containing the prices of SOL, BTC, and ETH tokens.
   */

    // if cachedValue is JSON, parse it

  /**
   * Asynchronously fetches token accounts for a specific owner.
   *
   * @returns {Promise<any[]>} A promise that resolves to an array of token accounts.
   */

  /**
   * Update wallet data including fetching wallet portfolio information, prices, and caching the data.
   * @param {boolean} [force=false] - Whether to force update the wallet data even if the update interval has not passed
   * @returns {Promise<WalletPortfolio>} The updated wallet portfolio information
   */
    //console.log('updateWalletData - start')

      // can't be warn if we fire every start up
      // maybe we just get the pubkey here proper
      // or fall back to SOLANA_PUBLIC_KEY

    //console.log('updateWalletData - force', force, 'last', this.lastUpdate, 'UPDATE_INTERVAL', this.UPDATE_INTERVAL)
    // Don't update if less than interval has passed, unless forced
    //console.log('updateWalletData - fetch')

      // Try Birdeye API first
          //console.log('walletData', walletData)

            //console.log('saving portfolio', portfolio.items.length, 'tokens')

            // maybe should be keyed by public key

      // Fallback to basic token account info

  /**
   * Retrieves cached wallet portfolio data from the database adapter.
   * @returns A promise that resolves with the cached WalletPortfolio data if available, otherwise resolves with null.
   */

  /**
   * Forces an update of the wallet data and returns the updated WalletPortfolio object.
   * @returns A promise that resolves with the updated WalletPortfolio object.
   */

  /**
   * Retrieves the public key of the instance.
   *
   * @returns {PublicKey} The public key of the instance.
   */

  /**
   * Retrieves the connection object.
   *
   * @returns {Connection} The connection object.
   */

  /**
   * Validates a Solana address.
   * @param {string | undefined} address - The address to validate.
   * @returns {boolean} True if the address is valid, false otherwise.
   */
      // Handle Solana addresses

  /**
   * Creates a new Solana wallet by generating a keypair
   * @returns {Promise<{publicKey: string, privateKey: string}>} Object containing base58-encoded public and private keys
   */
      // Generate new keypair

      // Convert to base58 strings for secure storage

      // Clear the keypair from memory

  /**
   * Registers a provider with the service.
   * @param {any} provider - The provider to register
   * @returns {Promise<number>} The ID assigned to the registered provider
   */

  /**
   * Subscribes to account changes for the given public key
   * @param {string} accountAddress - The account address to subscribe to
   * @returns {Promise<number>} Subscription ID
   */

      // Check if already subscribed

      // Create WebSocket connection if needed

      // Setup notification handler
            // Force update wallet data to reflect changes

            // Emit an event that can be handled by the agent

  /**
   * Unsubscribes from account changes
   * @param {string} accountAddress - The account address to unsubscribe from
   * @returns {Promise<boolean>} Success status
   */

  /**
   * Calculates the optimal buy amount and slippage based on market conditions
   * @param {JupiterService} jupiterService - Jupiter service instance
   * @param {string} inputMint - Input token mint address
   * @param {string} outputMint - Output token mint address
   * @param {number} availableAmount - Available amount to trade
   * @returns {Promise<{ amount: number; slippage: number }>} Optimal amount and slippage
   */
      // Get price impact for the trade

      // Find optimal slippage based on market conditions

      // If price impact is too high, reduce the amount
        // 5% price impact threshold

  /**
   * Executes buy orders for multiple wallets
   * @param {Array<{ keypair: any; balance: number }>} wallets - Array of wallet information
   * @param {any} signal - Trading signal information
   * @returns {Promise<Array<{ success: boolean; outAmount?: number; fees?: any; swapResponse?: any }>>}
   */

        // Get initial quote to determine input mint and other parameters

        // Calculate optimal buy amount using the input mint from quote

        // Get final quote with optimized amount

        // Execute the swap

        // Calculate final amounts including fees

/**
 * Gets either a keypair or public key based on TEE mode and runtime settings
 * @param runtime The agent runtime
 * @param requirePrivateKey Whether to return a full keypair (true) or just public key (false)
 * @returns KeypairResult containing either keypair or public key
 */
std::future<WalletResult> loadWallet(IAgentRuntime runtime, boolean = true requirePrivateKey);
std::future<std::string> sendTransaction(Connection connection, const std::vector<std::any>& instructions, Keypair wallet);

} // namespace elizaos
