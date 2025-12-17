#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "service.hpp"

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
  abstract addLiquidity(params: getDexName();


} // namespace elizaos
