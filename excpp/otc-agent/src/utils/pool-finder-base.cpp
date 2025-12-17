#include "pool-finder-base.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<std::optional<PoolInfo>> findBestPool(const std::string& tokenAddress, double chainId = 8453) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto cacheKey = "pool:" + std::to_string(chainId) + ":" + std::to_string(tokenAddress.toLowerCase());

        // Check cache first
        const auto cached = getCached<PoolInfo | nullptr>(cacheKey);
        if (cached != undefined) {
            return cached;
        }

        const auto config = CONFIG[chainId];
        if (!config) throw new Error(`Unsupported chain ID: ${chainId}`);

        // Determine chain based on chainId
        const auto chain =;
        chainId == 84532;
        ? baseSepolia;
        : chainId == 56
        ? bsc;
        : chainId == 97
        ? bscTestnet;
        : base;

        // Create client with explicit type to avoid deep type instantiation
        const auto client = createPublicClient({;
            chain,
            transport: http(config.rpcUrl),
            });

            const auto promises = [findUniswapV3Pools(client, tokenAddress, config)];

            // Aerodrome Slipstream (CL) pools - compatible with SimplePoolOracle (Uniswap V3 interface)
            // Note: Aerodrome V2 pools (Basic/Volatile) do NOT support the IUniswapV3Pool interface
            if (config.aerodromeCLFactory) {
                promises.push(findAerodromeCLPools(client, tokenAddress, config));
            }

            if (config.pancakeswapFactory) {
                promises.push(findPancakeswapPools(client, tokenAddress, config));
            }

            const auto results = Promise.all(promises);
            const auto allPools = results.flat();

            if (allPools.length == 0) {
                // Cache null result too
                setCache(cacheKey, nullptr, POOL_CACHE_TTL_MS);
                return nullptr;
            }

            // Sort by TVL descending
            allPools.sort((a, b) => b.tvlUsd - a.tvlUsd);

            // Return pool with highest TVL
            const auto bestPool = allPools[0];
            setCache(cacheKey, bestPool, POOL_CACHE_TTL_MS);
            return bestPool;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

void validatePoolLiquidity(PoolInfo pool) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    valid: boolean;
    warning?: string;

}

std::string formatPoolInfo(PoolInfo pool) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (pool.protocol == "Aerodrome") {
        const auto type = pool.stable ? "Stable" : "Volatile";
        return "Aerodrome " + std::to_string(type) + " (" + std::to_string(pool.baseToken) + ") - TVL: ~$" + std::to_string(Math.floor(pool.tvlUsd).toLocaleString());
    }
    const auto feePercent = ((pool.fee || 0) / 10000).toFixed(2);
    return std::to_string(pool.protocol) + " (" + std::to_string(feePercent) + "%, " + std::to_string(pool.baseToken) + ") - TVL: ~$" + std::to_string(Math.floor(pool.tvlUsd).toLocaleString());

}

} // namespace elizaos
