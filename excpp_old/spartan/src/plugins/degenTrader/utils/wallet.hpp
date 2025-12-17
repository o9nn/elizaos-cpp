#pragma once
#include <any>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "elizaos/core.hpp"
#include "utils.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Gets wallet keypair from runtime settings
 * @param runtime Agent runtime environment
 * @returns Solana keypair for transactions
 * @throws Error if private key is missing or invalid
 */
Keypair getWalletKeypair(std::optional<IAgentRuntime> runtime);

/**
 * Gets current SOL balance for wallet
 * @param runtime Agent runtime environment
 * @returns Balance in SOL
 */
std::future<double> getWalletBalance(IAgentRuntime runtime);

// Add helper function to get connection
std::future<Connection> getConnection(IAgentRuntime runtime);

// Add configuration constants
  // Exponential backoff between retries

// Add function to calculate dynamic slippage
double calculateDynamicSlippage(const std::string& amount, const std::any& quoteData);

/**
 * Execute a trade with detailed logging
 */

    // Setup swap parameters

    // Convert amount to lamports for the API

    /*
    */

    // Get quote using Jupiter API

    // 100 is 1%, we don't need to scale it by 10k
    // 10k is if we gave it 0.01 to mean 1%

      // {\"error\":\"The token GkpRhRXqVYUJS2gMPLyjefAcwLMCLwoxaBKRr1ghnu1k is not tradable\",\"errorCode\":\"TOKEN_NOT_TRADABLE\"}
        void extractTokenAddress(auto message);
        //

    // Calculate dynamic slippage based on market conditions

    // Update quote with dynamic slippage

    // Execute transaction

    // Get fresh blockhash with processed commitment for speed

    // Send transaction

    // Improve confirmation checking with exponential backoff

        // Calculate delay with exponential backoff

        // Wait before retry with exponential backoff

    // Get quote from Raydium API

    // Get swap transaction

    // Execute transaction

    // Get fresh blockhash with processed commitment for speed

    // Send transaction

    // Improve confirmation checking

std::future<double> getChainWalletBalance(IAgentRuntime runtime, const std::string& _tokenAddress);

// Add this helper function at the top level
std::future<std::string> simulateTransaction(const std::any& client, const std::any& tx);

struct TokenBalance {
    std::string mint;
    double balance;
    double decimals;
    double uiAmount;
};

/**
 * Gets all token balances for a wallet including SOL and SPL tokens
 */
std::future<void> getWalletBalances(IAgentRuntime runtime);

/**
 * Gets balance of a specific token
 */

/**
 * Checks if wallet has any balance of a specific token
 */
std::future<bool> hasTokenBalance(IAgentRuntime runtime, const std::string& tokenMint);

} // namespace elizaos
