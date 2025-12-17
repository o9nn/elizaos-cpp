#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include "retry-cache.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Type-safe wrapper for readContract calls in this module.
 * viem 2.40+ has strict generics that require compile-time ABI inference.
 * This wrapper allows us to use dynamic ABIs while maintaining type safety on return values.
 */
  // Cast is necessary for dynamic ABIs - viem's generics require compile-time inference

// Cache TTL for pool info (30 seconds)

// Configuration by Chain ID
  // Base Mainnet
    // Aerodrome Slipstream (Velodrome CL) PoolFactory - verified from official deployment
    // https://github.com/velodrome-finance/slipstream
    // Fallback RPC for Base Mainnet to avoid rate limits on public endpoint
  // Base Sepolia
    // Aerodrome not officially on Sepolia, using same address will fail, so undefined
  // BSC Mainnet
  // BSC Testnet

// Uniswap: 100 (0.01%), 500 (0.05%), 3000 (0.3%), 10000 (1%)
// PancakeSwap: 100 (0.01%), 500 (0.05%), 2500 (0.25%), 10000 (1%)
// Aerodrome CL: Uses tickSpacing instead of fee. Common: 1, 50, 100, 200, 2000

struct PoolInfo {
    std::string address;
    std::string token0;
    std::string token1;
    std::optional<double> fee;
    std::optional<double> tickSpacing;
    std::optional<bool> stable;
    bigint liquidity;
    double tvlUsd;
    std::optional<double> priceUsd;
    std::variant<"USDC", "WETH"> baseToken;
};

/**
 * Find all pools (Uniswap V3 + Aerodrome) for a given token
 * @param tokenAddress The token to find pools for
 * @param chainId The chain ID to search on (default: Base Mainnet 8453)
 * @returns Array of pool information sorted by TVL
 */

/**
 * Find Uniswap V3 pools with retry and caching
 */

  // Helper to check a pool with retry

      // Silently ignore pool check errors (pool doesn't exist or RPC error)

  // Check all combinations
  // Only check USDC if NOT on BSC, because BSC USDC is 18 decimals and breaks SimplePoolOracle
    // BSC: Only check WETH (WBNB)

/**
 * Find Aerodrome Slipstream (CL) pools
 * These are compatible with SimplePoolOracle (Uniswap V3 interface)
 * Uses the official Velodrome Slipstream PoolFactory
 */

  // Helper to check a pool with retry

        // Use getUniPoolInfo because CL pools share the same interface as Uni V3
      // Silently ignore pool check errors (pool doesn't exist)

  // Check all combinations
  // On Base, check both USDC and WETH

/**
 * Find Aerodrome pools (Stable + Volatile)
 * @internal Currently disabled pending token-agnostic deployment
 */
// eslint-disable-next-line @typescript-eslint/no-unused-vars

  // Helper to check a pool

  // Check Stable and Volatile for both base tokens

/**
 * Find PancakeSwap V3 pools (Same interface as Uniswap V3) with retry and caching
 */

  // Helper to check a pool with retry

      // Ignore pool lookup errors

  // Check all combinations
  // Only check USDC if NOT on BSC, because BSC USDC is 18 decimals and breaks SimplePoolOracle
    // BSC: Only check WETH (WBNB)

/**
 * Get detailed information about a Uniswap V3 pool
 */

  // Fetch decimals for price calculation

  // Calculate real TVL using liquidity and sqrtPrice
  // slot0 returns a tuple: [sqrtPriceX96, tick, observationIndex, ...]

  // Calculate Price in USD
  // Price of Token0 in terms of Token1 = (sqrtPriceX96 / 2^96)^2
  // Adjusted Price = Price * 10^(decimals0 - decimals1)
  // We want price of TargetToken in USD.

  // P = price of Token0 in Token1 (raw)
  // Use number for price calc (precision loss acceptable for validation)

  // Adjust for decimals: Price of 1 unit of Token0 = X units of Token1
  // Real Price 0 in 1 = price0in1 * 10^(decimals0 - decimals1) ??
  // Formula: price0 = (sqrtP^2) * (10^dec0 / 10^dec1) ??
  // Standard: price0 = price0in1 / (10^dec1 / 10^dec0) = price0in1 * 10^(dec0 - dec1)

  // Base Token Price (USDC or ETH)

    // Target is Token0. We want price of Token0 in USD.
    // If Token1 is Base (Quote): Price0 = Price0in1 * PriceBase
    // Target is Token1. We want price of Token1 in USD.
    // Price1in0 = 1 / Price0in1
    // Price1 = (1 / price0in1Adjusted) * PriceBase

/**
 * Get detailed information about an Aerodrome pool
 */

  // Determine which reserve corresponds to the base token to estimate TVL
  // Note: This is a simplification. In production we should check decimals.

  // Estimate TVL: Base Reserve * 2 (assuming 50/50 pool value)
  // This works for both volatile and stable pools roughly

  // Normalize liquidity for the estimateTVL function which expects "units" roughly matching V3
  // But for V2 style, we can just calculate USD value directly

/**
 * Calculate V3 TVL in USD
 */

  // Calculate amount of base token in the pool
  // L = sqrt(x * y)
  // sqrtP = y / x (price of 0 in terms of 1? No, price of 0 in terms of 1 is y/x if 0 is base... wait)
  // Uniswap Price P is amount of token1 per 1 token0.
  // P = y / x. sqrtP = sqrt(y/x).
  // sqrtPriceX96 = sqrt(P) * 2^96.

  // If Token0 is Base (x is Base):
  // We want x.
  // x = L / sqrtP = L * 2^96 / sqrtPriceX96.

  // If Token1 is Base (y is Base):
  // We want y.
  // y = L * sqrtP = L * sqrtPriceX96 / 2^96.

    // Base is x
    // Base is y

  // Convert to USD
  // TVL is roughly 2x the base token amount (50/50 ratio in V3 at current tick range approx)
  // Actually V3 is concentrated, but for ranking, assuming 2x base is fair enough vs Aerodrome.

    // 6 decimals
    // 18 decimals (WETH/WBNB)

/**
 * Validate pool has sufficient liquidity
 */

/**
 * Format pool info for display
 */
std::string formatPoolInfo(PoolInfo pool);

} // namespace elizaos
