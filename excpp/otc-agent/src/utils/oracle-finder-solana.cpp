#include "oracle-finder-solana.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<std::optional<SolanaOracleInfo>> findSolanaOracle(const std::string& tokenMint) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Try Pyth first (most reliable)
    const auto pythFeed = findPythFeed(tokenMint);
    if (pythFeed) {
        return pythFeed;
    }

    // Try Jupiter aggregator
    const auto jupiterPool = findJupiterPool(tokenMint);
    if (jupiterPool) {
        return jupiterPool;
    }

    // Try Raydium pools
    const auto raydiumPool = findRaydiumPool(tokenMint);
    if (raydiumPool) {
        return raydiumPool;
    }

    return nullptr;

}

std::future<std::optional<SolanaOracleInfo>> findPythFeed(const std::string& tokenMint) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Query Pyth API for price feed
    const auto response = fetch(;
    "https://hermes.pyth.network/api/latest_price_feeds?ids[]=" + std::to_string(tokenMint)
    );

    if (!response.ok) {
        return nullptr;
    }

    const auto data = response.json();

    if (data && data.length > 0) {
        const auto feed = data[0];
        return {
            type: "pyth",
            address: PYTH_PROGRAM_ID,
            feedId: feed.id,
            valid: true,
            };
        }

        // Also try querying by symbol if mint doesn't work
        // This requires additional metadata lookup
        return nullptr;

}

std::future<std::optional<SolanaOracleInfo>> findJupiterPool(const std::string& tokenMint) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Query Jupiter API for available routes
    const auto response = fetch(;
    "https://quote-api.jup.ag/v6/quote?inputMint=" + std::to_string(tokenMint) + "&outputMint=EPjFWdd5AufqSSqeM2qN1xzybapC8G4wEGGkZwyTDt1v&amount=1000000"
    );

    if (!response.ok) {
        return nullptr;
    }

    const auto data = response.json();

    if (data && data.routePlan && data.routePlan.length > 0) {
        // Extract pool address from route
        const auto firstRoute = data.routePlan[0];

        return {
            type: "jupiter",
            address: JUPITER_PROGRAM_ID,
            poolAddress: firstRoute.swapInfo.ammKey || "unknown",
            valid: true,
            };
        }

        return nullptr;

}

std::future<std::optional<SolanaOracleInfo>> findRaydiumPool(const std::string& tokenMint) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Query Raydium API for pools (RPC connection not needed for API calls)
    const auto response = fetch("https://api.raydium.io/v2/main/pairs");

    if (!response.ok) {
        return nullptr;
    }

    interface RaydiumPool {
        baseMint: string;
        quoteMint: string;
        ammId: string;
        liquidity?: string;
    }
    const std::vector<RaydiumPool> pools = response.json();

    // Find pool with this token
    const auto pool = pools.find(;
    [&](p) { return p.baseMint == tokenMint || p.quoteMint == tokenMint,; }
    );

    if (pool) {
        // Validate pool has liquidity
        const auto liquidity = parseFloat(pool.liquidity || "0");

        if (liquidity < 50000) {
            return {
                type: "raydium",
                address: pool.ammId,
                poolAddress: pool.ammId,
                liquidity,
                valid: false,
                "Low liquidity: $" + std::to_string(liquidity.toLocaleString()) + ". Minimum recommended: $50,000"
                };
            }

            return {
                type: "raydium",
                address: pool.ammId,
                poolAddress: pool.ammId,
                liquidity,
                valid: true,
                };
            }

            return nullptr;

}

void validateSolanaOracle(SolanaOracleInfo oracle) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    valid: boolean;
    message: string;

}

std::string formatOracleInfo(SolanaOracleInfo oracle) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto tvl = oracle.liquidity;
    " - TVL: $" + std::to_string(oracle.liquidity.toLocaleString())
    : "";

    switch (oracle.type) {
        case "pyth":
        return "Pyth Price Feed (Most Reliable)";
        case "jupiter":
        return "Jupiter Aggregator";
        case "raydium":
        return "Raydium Pool" + std::to_string(tvl);
        case "orca":
        return "Orca Whirlpool" + std::to_string(tvl);
        case "pumpswap":
        return "PumpSwap Bonding Curve" + std::to_string(tvl);
        default:
        return "Unknown Oracle";
    }

}

void getSolanaRegistrationCost() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    sol: number;
    usd: number;
    breakdown: {
        accountCreation: number;
        programExecution: number;
        infrastructure: number;
        };

}

} // namespace elizaos
