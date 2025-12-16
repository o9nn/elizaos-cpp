#include "service.hpp"
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
 * A standardized representation of a liquidity pool from any DEX.
 */
using PoolInfo = {

/**
 * A standardized representation of a user's position in a liquidity pool.
 */
using LpPositionDetails = {

/**
 * A standardized result for blockchain transactions.
 */
using TransactionResult = {

/**
 * Abstract interface for a Liquidity Pool Service.
 * DEX-specific plugins (e.g., for Orca, Raydium) must implement this service
 * to allow the LP Manager to interact with them in a standardized way.
 */

  /**
   * Returns the name of the DEX this service interacts with.
   * @returns The name of the DEX (e.g., "Orca", "Raydium").
   */

  /**
   * Fetches a list of available liquidity pools from the DEX.
   * @param tokenAMint - Optional: Filter pools by the mint address of the first token.
   * @param tokenBMint - Optional: Filter pools by the mint address of the second token.
   * @returns A promise that resolves to an array of standardized PoolInfo objects.
   */

  /**
   * Adds liquidity to a specified pool.
   * @param params - The parameters for adding liquidity.
   * @returns A promise resolving to a transaction result, including the LP tokens received.
   */

  /**
   * Removes liquidity from a specified pool.
   * @param params - The parameters for removing liquidity.
   * @returns A promise resolving to a transaction result, including the tokens received.
   */

  /**
   * Fetches the details of a specific LP position for a user.
   * @param userAccountPublicKey - The user's wallet public key.
   * @param poolOrPositionIdentifier - The identifier for the pool or a specific position (e.g., position NFT mint).
   * @returns A promise resolving to the position details or null if not found.
   */

  /**
   * Fetches the latest market data (e.g., APY, TVL) for a list of pools.
   * @param poolIds - An array of pool IDs to fetch data for.
   * @returns A promise resolving to a map of pool IDs to their partial market data.
   */

} // namespace elizaos
