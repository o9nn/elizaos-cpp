#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

// Connection type not currently used but kept for std::future RPC calls
// 

struct SolanaOracleInfo {
    "pyth" | "jupiter" | "raydium" | "orca" | "pumpswap" type;
    std: address;
    std::optional<std:; // For Pyth> feedId;
    std::optional<std:; // For AMM pools> poolAddress;
    std::optional<double> liquidity;
    bool valid;
    std::optional<std:> warning;
};

/**
 * Find price oracle for a Solana token
 * Priority: Pyth > Jupiter > Raydium
 */
std::future<SolanaOracleInfo | null> findSolanaOracle(const std:& tokenMint);

/**
 * Check if token has a Pyth price feed
 * Pyth is the preferred oracle for Solana
 */
std::future<SolanaOracleInfo | null> findPythFeed(const std:& tokenMint);

/**
 * Find Jupiter aggregator pool for token
 */
std::future<SolanaOracleInfo | null> findJupiterPool(const std:& tokenMint);

/**
 * Find Raydium pool for token
 */
std::future<SolanaOracleInfo | null> findRaydiumPool(const std:& tokenMint);

/**
 * Validate oracle has sufficient data/liquidity
 */
void validateSolanaOracle(SolanaOracleInfo oracle); {

    // PumpSwap/Pump.fun bonding curves typically have lower liquidity but are still valid

/**
 * Format oracle info for display
 */
std: formatOracleInfo(SolanaOracleInfo oracle);

/**
 * Get registration cost estimate for Solana
 */
void getSolanaRegistrationCost(); {

} // namespace elizaos
