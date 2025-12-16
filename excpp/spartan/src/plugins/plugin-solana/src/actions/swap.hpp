#include ".constants.hpp"
#include ".keypairUtils.hpp"
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



/**
 * Fetches the number of decimals for a given token mint address.
 *
 * @param {Connection} connection - Solana connection object.
 * @param {string} mintAddress - Address of the token mint.
 * @returns {Promise<number>} - Number of decimals for the token.
 * @throws {Error} - If unable to fetch token decimals.
 */
std::future<double> getTokenDecimals(Connection connection, const std::string& mintAddress);

/**
 * Swaps tokens using the specified connection, wallet public key, input and output token contract addresses,
 * and amount. Returns a Promise that resolves to the swap data.
 *
 * @param {Connection} connection The connection object to use for interacting with the blockchain.
 * @param {PublicKey} walletPublicKey The public key of the wallet initiating the swap.
 * @param {string} inputTokenCA The contract address of the input token.
 * @param {string} outputTokenCA The contract address of the output token.
 * @param {number} amount The amount of tokens to swap.
 * @returns {Promise<unknown>} A Promise that resolves to the swap data object.
 */
std::future<unknown> swapToken(Connection connection, PublicKey walletPublicKey, const std::string& inputTokenCA, const std::string& outputTokenCA, double amount);

// Get token from wallet data using SolanaService
/**
 * Retrieves the token address from the wallet for the specified token symbol.
 *
 * @param {IAgentRuntime} runtime - The agent runtime.
 * @param {string} tokenSymbol - The token symbol to retrieve the address for.
 * @returns {Promise<string|null>} The token address if found, null otherwise.
 */
std::future<string | null> getTokenFromWallet(IAgentRuntime runtime, const std::string& tokenSymbol);

/**
 * Respond with a JSON markdown block containing only the extracted values. Use null for any values that cannot be determined.
 *
 * Example response:
 * ```json
 * {
 *     "inputTokenSymbol": "SOL",
 *     "outputTokenSymbol": "USDC",
 *     "inputTokenCA": "So11111111111111111111111111111111111111112",
 *     "outputTokenCA": "EPjFWdd5AufqSSqeM2qN1xzybapC8G4wEGGkZwyTDt1v",
 *     "amount": 1.5
 * }
 * ```
 *
 * {{recentMessages}}
 *
 * Given the recent messages and wallet information below:
 *
 * {{walletInfo}}
 *
 * Extract the following information about the requested token swap:
 * - Input token symbol (the token being sold)
 * - Output token symbol (the token being bought)
 * - Input token contract address if provided
 * - Output token contract address if provided
 * - Amount to swap
 *
 * Respond with a JSON markdown block containing only the extracted values. Use null for any values that cannot be determined.
 */

/**
 * Action for executing a token swap from one token to another on Solana.
 *
 * @type {Action}
 * @property {string} name - The name of the action ("SWAP_SOLANA").
 * @property {string[]} similes - Alternative names for the action.
 * @property {Function} validate - Asynchronous function to validate if Solana service is available.
 * @property {string} description - Description of the action.
 * @property {Function} handler - Asynchronous function to handle the token swap process.
 * @property {ActionExample[][]} examples - Examples demonstrating how to use the action.
 */

      // Handle SOL addresses

      // Resolve token addresses if needed


} // namespace elizaos
