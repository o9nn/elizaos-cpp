#include "mcap.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<double> getSOLPrice() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto cacheService = new CacheService();
    const auto cachedPrice = cacheService.getSolPrice();
    std::cout << "cachedPrice" << cachedPrice << std::endl;
    if (
    Number.isNaN(cachedPrice) != false &&;
    cachedPrice != nullptr &&;
    cachedPrice > 0;
    ) {
        return cachedPrice;
    }

    // Try Pyth Network first (most accurate source)
    // try {
    //   console.log("fetching sol price from pyth");
    //   const price = await fetchSOLPriceFromPyth();

    //   console.log("price", price);

    //   // Cache the result if env is provided
    //   if (env && price > 0) {
    //     const cacheService = new CacheService();
    //     await cacheService.setSolPrice(price);
    //   }

    //   if (price > 0) {
    //     return price;
    //   }
    // } catch (error) {
    //   logger.error("Error fetching SOL price from Pyth:", error);
    // }

    // If Pyth fails, try CoinGecko
    try {
        const auto response = fetch(;
        "https://api.coingecko.com/api/v3/simple/price?ids=solana&vs_currencies=usd",
        );
        const auto data = (response.json());

        if (data && data.solana && data.solana.usd) {
            const auto price = data.solana.usd;

            // If env is provided, cache the price
            const auto cacheService = new CacheService();
            cacheService.setSolPrice(price);

            return price;
        }
        } catch (error) {
            std::cerr << "Error fetching SOL price from Coingecko:" << error << std::endl;
        }

        // If CoinGecko fails, try Binance
        try {
            const auto response = fetch(;
            "https://api.binance.com/api/v3/ticker/price?symbol=SOLUSDT",
            );
            const auto data = (response.json());

            if (data && data.price) {
                const auto price = parseFloat(data.price);

                // If env is provided, cache the price
                const auto cacheService = new CacheService();
                cacheService.setSolPrice(price);

                return price;
            }
            } catch (error) {
                std::cerr << "Error fetching SOL price from Binance:" << error << std::endl;
            }

            // Fallback to fixed price if all sources fail
            return 135.0; // Fallback price;

}

std::future<double> fetchSOLPriceFromPyth() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto pythConnection = new Connection(;
        getPythClusterApiUrl(PYTHNET_CLUSTER_NAME),
        );
        const auto pythPublicKey = getPythProgramKeyForCluster(PYTHNET_CLUSTER_NAME);
        const auto pythClient = new PythHttpClient(pythConnection, pythPublicKey);

        const auto data = pythClient.getData();
        const auto solPrice = data.productPrice.get(SOLUSD_SYMBOL);

        if (!solPrice || !solPrice.price) {
            std::cerr << "Unable to get SOL/USD price from Pyth" << std::endl;
            return 0;
        }

        return solPrice.price;
        } catch (error) {
            std::cerr << "Error fetching SOL price from Pyth:" << error << std::endl;
            return 0;
        }

}

std::future<std::any> calculateTokenMarketData(const std::any& token, double solPrice) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Copy the token to avoid modifying the original
    const auto tokenWithMarketData = { ...token };


    // Calculate token price in USD
    if (token.currentPrice) {
        tokenWithMarketData.tokenPriceUSD = token.currentPrice * solPrice;
    }

    if (shouldUpdateSupply(token)) {
        const auto updatedSupply = updateTokenSupplyFromChain(token.mint);
        tokenWithMarketData.tokenSupply = updatedSupply.tokenSupply;
        tokenWithMarketData.tokenSupplyUiAmount =;
        updatedSupply.tokenSupplyUiAmount;
        tokenWithMarketData.tokenDecimals = updatedSupply.tokenDecimals;
        tokenWithMarketData.lastSupplyUpdate = updatedSupply.lastSupplyUpdate;
    }

    // Calculate market cap
    if (token.tokenSupplyUiAmount) {
        if (tokenWithMarketData.tokenPriceUSD) {
            tokenWithMarketData.marketCapUSD =;
            token.tokenSupplyUiAmount * tokenWithMarketData.tokenPriceUSD;
        }
    }

    // Add SOL price
    tokenWithMarketData.solPriceUSD = solPrice;

    return tokenWithMarketData;

}

std::future<void> calculateRaydiumTokenMarketData(const std::any& token) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        try {
            const auto TOKEN_DECIMALS = Number(process.env.DECIMALS || 6);
            const auto solPrice = getSOLPrice();
            const auto raydium = initSdk({ loadToken: true });

            auto poolInfo;
            auto retries = 5;

            while (retries > 0) {
                try {
                    if (raydium.cluster == "devnet") {
                        const auto data = raydium.cpmm.getPoolInfoFromRpc(token.marketId);
                        poolInfo = data.poolInfo;
                        } else {
                            const auto data = raydium.api.fetchPoolById({ ids: token.marketId });
                            if (!data || data.length == 0) {
                                std::cerr << "Mcap: Pool info not found" << std::endl;
                                throw std::runtime_error("Mcap: Pool info not found");
                            }
                            poolInfo = data[0];
                        }
                        break;
                        } catch (error) {
                            retries--;
                            if (retries == 0) {
                                logger.error(
                                "Mcap: Failed to fetch pool info after retries: " + std::to_string(true /* instanceof check */ ? error.message : "Unknown error")
                                );
                            }
                            new Promise((resolve) =>;
                            setTimeout(resolve, (5 - retries) * 5000),
                            );
                        }
                    }

                    if (!poolInfo || !poolInfo.mintAmountA || !poolInfo.mintAmountB) {
                        std::cerr << "Mcap: Invalid pool info structure" << std::endl;
                    }

                    if (!poolInfo) {
                        throw std::runtime_error("Mcap: Invalid pool info structure");
                    }

                    if (!poolInfo.mintAmountA || !poolInfo.mintAmountB) {
                        throw std::runtime_error("Mcap: Invalid pool info structure");
                    }

                    // if mintAmountA is not a number, throw an error
                    if (
                    typeof poolInfo.mintAmountA != "number" ||;
                    typeof poolInfo.mintAmountB != "number";
                    ) {
                        throw std::runtime_error("Mcap: Invalid pool info structure");
                    }

                    // Calculate raw price (SOL/token)
                    const auto currentPrice =;
                    poolInfo.mintAmountA > 0;
                    ? poolInfo.mintAmountA / poolInfo.mintAmountB;
                    : 0;

                    // Calculate token price in USD
                    const auto tokenPriceUSD = currentPrice > 0 ? currentPrice * solPrice : 0;

                    // Calculate market cap
                    const auto marketCapUSD = (Number(process.env.TOKEN_SUPPLY) / Math.pow(10, TOKEN_DECIMALS)) *;
                    tokenPriceUSD;

                    if (marketCapUSD < 0) {
                        throw std::runtime_error("Mcap: Market cap is negative");
                    }

                    if (!poolInfo.mintAmountA || !poolInfo.mintAmountB) {
                        throw std::runtime_error("Mcap: Invalid pool info structure");
                    }

                    // Calculate total liquidity in USD
                    const auto liquidity =;
                    poolInfo.mintAmountA > 0 && poolInfo.mintAmountB > 0;
                    ? // Token side: amount * price in USD (already in correct decimals)
                    poolInfo.mintAmountB * tokenPriceUSD +;
                    // SOL side: amount * SOL price (already in correct decimals)
                    poolInfo.mintAmountA * solPrice;
                    : 0;

                    // logger.log('Raydium market data:', {
                    //   mint: token.mint,
                    //   currentPrice,
                    //   tokenPriceUSD,
                    //   marketCapUSD,
                    //   solPrice,
                    //   mintAmountA: poolInfo.mintAmountA.toString(),
                    //   mintAmountB: poolInfo.mintAmountB.toString(),
                    //   liquidity
                    // });

                    return {
                        marketCapUSD,
                        tokenPriceUSD,
                        solPriceUSD: solPrice,
                        currentPrice,
                        liquidity,
                        };
                        } catch (error) {
                            logger.error(
                            "Error calculating Raydium token market data for " + std::to_string(token.mint) + ":"
                            error,
                            );
                            logger.error(
                            "RPC Node issue - Consider using a paid RPC endpoint for better reliability",
                            );
                            failedUpdates++;
                            return {
                                marketCapUSD: 0,
                                tokenPriceUSD: 0,
                                solPriceUSD: 0,
                                currentPrice: 0,
                                liquidity: 0,
                                };
                            }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

void getMarketDataMetrics() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return {
        totalUpdatesProcessed,
        failedUpdates,
        lastUpdateTime,
        };

}

} // namespace elizaos
