#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

// Connection type not currently used but kept for future RPC calls
// 

struct SolanaOracleInfo {
    std::variant<"pyth", "jupiter", "raydium", "orca", "pumpswap"> type;
    std::string address;
    std::optional<std::string> feedId;
    std::optional<std::string> poolAddress;
    std::optional<double> liquidity;
    bool valid;
    std::optional<std::string> warning;
};

/**
 * Find price oracle for a Solana token
 * Priority: Pyth > Jupiter > Raydium
 */

/**
 * Check if token has a Pyth price feed
 * Pyth is the preferred oracle for Solana
 */

/**
 * Find Jupiter aggregator pool for token
 */

/**
 * Find Raydium pool for token
 */

/**
 * Validate oracle has sufficient data/liquidity
 */

    // PumpSwap/Pump.fun bonding curves typically have lower liquidity but are still valid

/**
 * Format oracle info for display
 */
std::string formatOracleInfo(SolanaOracleInfo oracle);

/**
 * Get registration cost estimate for Solana
 */

} // namespace elizaos
